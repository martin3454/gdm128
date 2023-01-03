
#include "MKL25Z4.h"
#include <glcd.h>


#define RS 		(1<<11)
#define E 		(1<<10)
#define CS1 	(1<<8)
#define CS2 	(1<<9)
#define RST 	(1<<12)
#define CLK 	(1<<17)
#define DAT 	(1<<31)
#define STR 	(1<<12)

#define delayEnable 15
#define delay 10

#define SET_X 		0x40
#define SET_Y 		0xB8
#define START_LINE 	0xc0
#define DIS_ON 		0x3f
#define DIS_WIDTH  	0x80
#define DIS_HEIGH  	0x40



uint8_t DDRAM[8][128];
uint8_t curX, curY;						//souradnice pozice
uint8_t barva; 							// 1 tmava, 0 bila
uint8_t pomX, pomY;						//pomocne souradnice pozice


void Ddram_WriteByte(uint8_t x, uint8_t y, uint8_t byte){

	for(uint8_t i = 0; i < 8; i++){
		barva = (1 << i) & byte;
		Ddram_Write(y + i, x);
	}
}


void DdramToLCD(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2){

	uint8_t x = x1;

	for(uint8_t i = y1; i < y2; i += 8){

		SetPosition(x, i);

		for(uint8_t j = x1; j < x2; j++){

			uint8_t temp = DDRAM[i/8][j];
			Glcd_WriteData(temp);

		}
	}
}


int abs(a){
	return (a < 0) ? -a : a;
}

void Ddram_Clear(){
	memset(DDRAM, 0, sizeof(DDRAM));
}

void Ddram_Set(){
	memset(DDRAM, 0xff, sizeof(DDRAM));
}

//funkce pro zapis pixelu do pole, tuto funkci upravit, pridat parametr pro 0,1
void Ddram_Write(uint8_t x, uint8_t y){

	if(x > 63 || y > 127) return;
	if(x < 0 || y < 0) return;

	if(barva) DDRAM[x/8][y] |= (1 << x % 8);
	else DDRAM[x/8][y] &= ~(1 << x % 8);

}

uint8_t Ddram_Read(){

	return DDRAM[curY][curX];
}

//incializace a zapnuti displeje
void GlcdInit(){

	Pin_Init();

	Glcd_WriteCommand(DIS_ON, 0);
	Glcd_WriteCommand(SET_X, 0);
	Glcd_WriteCommand(SET_Y, 0);
	Glcd_WriteCommand(START_LINE, 0);

	Glcd_WriteCommand(DIS_ON, 1);
	Glcd_WriteCommand(SET_X, 1);
	Glcd_WriteCommand(SET_Y, 1);
	Glcd_WriteCommand(START_LINE, 1);

	curX = curY = 0;
}


void SetColor(uint8_t col){

	if(col == 1 || col == 0) barva = col;
	else barva = 1;
}

//funkce pro mazani obrazovky
void ClearScreen(){

	uint8_t i, j;
	Ddram_Clear();

	for(i = 0;i < 64; i+= 8){

		SetPosition(0, i);

		for(j = 0;j < 128; j++){

			Glcd_WriteData(0x00);
		}
	}
}

//funkce pro zaplneni obrazovky
void FillScreen(){

	uint8_t i, j;
	Ddram_Set();

	for(i = 0; i < 64; i+= 8){

		SetPosition(0, i);

		for(j = 0;j < 128; j++){

			Glcd_WriteData(0xff);
		}
	}
}


//funkce pro zapis pozadovane polohy do radicu
void SetPosition(uint8_t col, uint8_t row){

	if(col > 127) col = 127;
	if(row > 63) row = 63;
	if(col < 0) col = 0;
	if(row < 0) row = 0;

	pomX = curX = col;				//nastaveni globalnich promennych na aktualni soradnice
	pomY = row;
	curY = row / 8;

	//cyklus init radicu, toto je pro pripad ze se prechazi ze 63 pozice na 64 na displeji
	//je proste potreba predem nastavit radice pro pozadovanou pozici, jinak se data nezobrazi nebo zobrazi spatne
	for(uint8_t i = 0; i < 2; i++){

		Glcd_WriteCommand(SET_X | 0, i);
		Glcd_WriteCommand(SET_Y | curY, i);
		Glcd_WriteCommand(START_LINE | 0, i);
	}

	//tady probiha zapis pozadovane polohy do radkoveho a strankove radice
	//vyber radice je podle podilu x/64 je to bud 0,1
	Glcd_WriteCommand(SET_X | (curX % 64), (curX / 64 ));
	Glcd_WriteCommand(SET_Y | curY, (curX / 64 ));

}

//KRESLICI FUNKCE

void SetPixel(uint8_t x, uint8_t y){

	uint8_t temp = 0;
	Ddram_Write(y, x);
	SetPosition(x, y);
	temp = Ddram_Read();
	Glcd_WriteData(temp);

}

void Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2){

	int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
	dx = x2 - x1;
	dy = y2 - y1;
	dx1 = abs(dx);
	dy1 = abs(dy);
	px = 2*dy1 - dx1;
	py = 2*dx1 - dy1;

	if(dy1 <= dx1){
		if(dx >= 0){
			x = x1;
			y = y1;
			xe = x2;
		}else{
			x = x2;
			y = y2;
			xe = x1;
		}

		SetPixel(x,y);

		while(x < xe){

			x = x + 1;
			if(px < 0) px = px + 2*dy1;
			else{
				if((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) y = y + 1;
				else y = y - 1;

				px = px + 2*(dy1 - dx1);
			}
			SetPixel(x,y);
		}
	}else{
		if(dy >= 0){
			x = x1;
			y = y1;
			ye = y2;
		}else{
			x = x2;
			y = y2;
			ye = y1;
		}

		SetPixel(x,y);

		while(y < ye){
			y = y + 1;
			if(py <= 0) py = py + 2*dx1;
			else{
				if((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) x = x + 1;
				else x = x - 1;

				py = py + 2*(dx1 - dy1);
			}
			SetPixel(x, y);
		}
	}
}




void Hline(uint8_t x1, uint8_t y1, uint8_t d){

	if(x1 > d) {
		uint8_t pom = x1;
		x1 = d;
		d = pom;
	}

	for(uint8_t j = x1; j < d; j++){
		SetPixel(j, y1);
	}
}

void Vline(uint8_t x1, uint8_t y1, uint8_t v){

	if(y1 > v){
		uint8_t pom = y1;
		y1 = v;
		v = pom;
	}

	for(uint8_t j = y1; j < v; j++){
		SetPixel(x1, j);
	}
}

//vykresleni obdelniku/ctverce
void Rectangle(uint8_t x, uint8_t y, uint8_t a, uint8_t b){

	uint8_t j;
	for (j = 0; j < b; j++) {
		SetPixel(x, y + j);
		SetPixel(x + a - 1, y + j);
	}

	for (j = 0; j < a; j++)	{
		SetPixel(x + j, y);
		SetPixel(x + j, y + b - 1);
	}

}

//vykresleni celeho obdelniku/ctverce
void FullRectangle(uint8_t x, uint8_t y, uint8_t a, uint8_t b){

	for(uint8_t j = 0; j < a; j++)
		Vline(x + j, y, y + b);
}

void Circle(uint8_t xc, uint8_t yc, uint8_t r){

	int pk, x, y;
	pk = 3 - 2 * r;
	x = 0;
	y = r;

	circle_points(x, y, xc, yc);

	while(x < y){

		if(pk < 0){
			pk = pk + 4*x + 6;
			circle_points(++x, y, xc, yc);
		}else{
			pk = pk + 4 * (x - y) + 10;
			circle_points(++x, --y, xc, yc);
		}
	}

}

//interni funkce pro Circle()
void circle_points(uint8_t x, uint8_t y, uint8_t xc, uint8_t yc){

	SetPixel(x + xc, y + yc);
	SetPixel(-x + xc, y + yc);
	SetPixel(x + xc, -y + yc);
	SetPixel(-x + xc, -y + yc);
	SetPixel(y + xc, x + yc);
	SetPixel(y + xc, -x + yc);
	SetPixel(-y + xc, x + yc);
	SetPixel(-y + xc, -x + yc);
}


//vykresleni plneho kruhu
void FullCircle(uint8_t x1, uint8_t y1, uint8_t r){

	for(int y = -r; y <= r; y++){

		for(int x = -r; x <= r; x++){

			if(x * x + y * y <= r * r){

				SetPixel(x1 + x, y1 + y);
			}
		}
	}

}


//funkce pro zapis prikazu do radice v parametru control
void Glcd_WriteCommand(uint8_t cmd, uint8_t control){

	SR_WriteData(cmd);
	//cekej(10);
	Glcd_RS_L();
	Glcd_EnableControler(control);
	Glcd_Enable_H();
	Wait_Nop(delayEnable);
	Glcd_Enable_L();
	Glcd_DisableControler(control);

}


//funkce pro zapis dat do radice na zaklade aktualniho kurzoru
void Glcd_WriteData(uint8_t dat){

	SR_WriteData(dat);
	//cekej(10);
	Glcd_RS_H();
	Glcd_EnableControler(curX / 64);
	Glcd_Enable_H();
	Wait_Nop(delayEnable);
	Glcd_Enable_L();
	Glcd_DisableControler(curX / 64);
	if(++curX >= DIS_WIDTH) curX = 0;
}


//funkce pro vyber a odpojeni radicu
void Glcd_EnableControler(uint8_t control){
	switch(control){
	case 0: PTC->PSOR = CS1;
	break;
	case 1: PTC->PSOR = CS2;
	break;
	}
}
void Glcd_DisableControler(uint8_t control){
	switch(control){
	case 0: PTC->PCOR = CS1;
	break;
	case 1: PTC->PCOR = CS2;
	break;
	}
}

// DATA SIGNAL
void Glcd_RS_H(){
	PTC->PSOR = RS;
}

//INS SIGNAL
void Glcd_RS_L(){
	PTC->PCOR = RS;
}

//ENABLE SIGNAL H
void Glcd_Enable_H(){
	PTC->PSOR = E;
}

//ENABLE SIGNAL L
void Glcd_Enable_L(){
	PTC->PCOR = E;
}

//INICIALIZACE PINU
void Pin_Init(){

	SIM->SCGC5 |= 1<<11;
	SIM->SCGC5 |= 1<<9;					//port A
	SIM->SCGC5 |= 1<<13;

	PORTC->PCR[8]= 0x100;
	PORTC->PCR[9]= 0x100;
	PORTC->PCR[10]= 0x100;
	PORTC->PCR[11]= 0x100;
	PORTC->PCR[12]= 0x100;
	PORTC->PCR[17]= 0x100;

	PORTA->PCR[12]= 0x100;
	PORTE->PCR[31]= 0x100;

	PTC->PDDR |= RS;
	PTC->PCOR = RS;

	PTC->PDDR |= E;
	PTC->PCOR = E;			//mozna SOR nwm

	PTC->PDDR |= CS1;
	PTC->PCOR = CS1;

	PTC->PDDR |= CS2;
	PTC->PCOR = CS2;

	PTC->PDDR |= STR;
	PTC->PCOR = STR;

	PTC->PDDR |= CLK;
	PTC->PCOR = CLK;

	PTA->PDDR |= RST;
	PTA->PSOR = RST;    	//vypnuti resetu

	PTE->PDDR |= DAT;
	PTE->PCOR = DAT;

}

//cekani za pomoci instrukce v asm nop...nedelat nic
void Wait_Nop(uint32_t cekej){

	static volatile uint32_t i;
	for(i = 0; i < cekej; i++);
		__asm("nop");

}


void SR_WriteData(uint8_t data){

	for(uint8_t i = 0 ; i < 8; i++){

		if((data << i) & 0b10000000){
			PTE->PSOR = DAT;		//SR_H
		}
		else{
			PTE->PCOR = DAT;		//SR_L
		}
								//HODINOVY SIGNAL SR_CLK();
		PTC->PCOR =CLK;
		PTC->PSOR =CLK;
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		__asm("nop");
		PTC->PCOR =CLK;
	}
								//STROBO SIGNAL	SR_STR();
	PTC->PCOR =STR;
	PTC->PSOR =STR;
	PTC->PCOR =STR;
}


//ZNAKOVA SADA A FUNKCE

const unsigned char font []= {
		0x00, 0x00, 0x00, 0x00, 0x00,// (mezera)
		0x00, 0x00, 0x5F, 0x00, 0x00,// !
		0x00, 0x07, 0x00, 0x07, 0x00,// "
		0x14, 0x7F, 0x14, 0x7F, 0x14,// #
		0x24, 0x2A, 0x7F, 0x2A, 0x12,// $
		0x23, 0x13, 0x08, 0x64, 0x62,// %
		0x36, 0x49, 0x55, 0x22, 0x50,// &
		0x00, 0x05, 0x03, 0x00, 0x00,// '
		0x00, 0x1C, 0x22, 0x41, 0x00,// (
		0x00, 0x41, 0x22, 0x1C, 0x00,// )
		0x08, 0x2A, 0x1C, 0x2A, 0x08,// *
		0x08, 0x08, 0x3E, 0x08, 0x08,// +
		0x00, 0x50, 0x30, 0x00, 0x00,// ,
		0x08, 0x08, 0x08, 0x08, 0x08,// -
		0x00, 0x30, 0x30, 0x00, 0x00,// .
		0x20, 0x10, 0x08, 0x04, 0x02,// /
		0x3E, 0x51, 0x49, 0x45, 0x3E,// 0
		0x00, 0x42, 0x7F, 0x40, 0x00,// 1
		0x42, 0x61, 0x51, 0x49, 0x46,// 2
		0x21, 0x41, 0x45, 0x4B, 0x31,// 3
		0x18, 0x14, 0x12, 0x7F, 0x10,// 4
		0x27, 0x45, 0x45, 0x45, 0x39,// 5
		0x3C, 0x4A, 0x49, 0x49, 0x30,// 6
		0x01, 0x71, 0x09, 0x05, 0x03,// 7
		0x36, 0x49, 0x49, 0x49, 0x36,// 8
		0x06, 0x49, 0x49, 0x29, 0x1E,// 9
		0x00, 0x36, 0x36, 0x00, 0x00,// :
		0x00, 0x56, 0x36, 0x00, 0x00,// ;
		0x00, 0x08, 0x14, 0x22, 0x41,// <
		0x14, 0x14, 0x14, 0x14, 0x14,// =
		0x41, 0x22, 0x14, 0x08, 0x00,// >
		0x02, 0x01, 0x51, 0x09, 0x06,// ?
		0x32, 0x49, 0x79, 0x41, 0x3E,// @
		0x7E, 0x11, 0x11, 0x11, 0x7E,// A
		0x7F, 0x49, 0x49, 0x49, 0x36,// B
		0x3E, 0x41, 0x41, 0x41, 0x22,// C
		0x7F, 0x41, 0x41, 0x22, 0x1C,// D
		0x7F, 0x49, 0x49, 0x49, 0x41,// E
		0x7F, 0x09, 0x09, 0x01, 0x01,// F
		0x3E, 0x41, 0x41, 0x51, 0x32,// G
		0x7F, 0x08, 0x08, 0x08, 0x7F,// H
		0x00, 0x41, 0x7F, 0x41, 0x00,// I
		0x20, 0x40, 0x41, 0x3F, 0x01,// J
		0x7F, 0x08, 0x14, 0x22, 0x41,// K
		0x7F, 0x40, 0x40, 0x40, 0x40,// L
		0x7F, 0x02, 0x04, 0x02, 0x7F,// M
		0x7F, 0x04, 0x08, 0x10, 0x7F,// N
		0x3E, 0x41, 0x41, 0x41, 0x3E,// O
		0x7F, 0x09, 0x09, 0x09, 0x06,// P
		0x3E, 0x41, 0x51, 0x21, 0x5E,// Q
		0x7F, 0x09, 0x19, 0x29, 0x46,// R
		0x46, 0x49, 0x49, 0x49, 0x31,// S
		0x01, 0x01, 0x7F, 0x01, 0x01,// T
		0x3F, 0x40, 0x40, 0x40, 0x3F,// U
		0x1F, 0x20, 0x40, 0x20, 0x1F,// V
		0x7F, 0x20, 0x18, 0x20, 0x7F,// W
		0x63, 0x14, 0x08, 0x14, 0x63,// X
		0x03, 0x04, 0x78, 0x04, 0x03,// Y
		0x61, 0x51, 0x49, 0x45, 0x43,// Z
		0x00, 0x00, 0x7F, 0x41, 0x41,// [
		0x02, 0x04, 0x08, 0x10, 0x20,// "\"
		0x41, 0x41, 0x7F, 0x00, 0x00,// ]
		0x04, 0x02, 0x01, 0x02, 0x04,// ^
		0x40, 0x40, 0x40, 0x40, 0x40,// _
		0x00, 0x01, 0x02, 0x04, 0x00,// `
		0x20, 0x54, 0x54, 0x54, 0x78,// a
		0x7F, 0x48, 0x44, 0x44, 0x38,// b
		0x38, 0x44, 0x44, 0x44, 0x20,// c
		0x38, 0x44, 0x44, 0x48, 0x7F,// d
		0x38, 0x54, 0x54, 0x54, 0x18,// e
		0x08, 0x7E, 0x09, 0x01, 0x02,// f
		0x08, 0x14, 0x54, 0x54, 0x3C,// g
		0x7F, 0x08, 0x04, 0x04, 0x78,// h
		0x00, 0x44, 0x7D, 0x40, 0x00,// i
		0x20, 0x40, 0x44, 0x3D, 0x00,// j
		0x00, 0x7F, 0x10, 0x28, 0x44,// k
		0x00, 0x41, 0x7F, 0x40, 0x00,// l
		0x7C, 0x04, 0x18, 0x04, 0x78,// m
		0x7C, 0x08, 0x04, 0x04, 0x78,// n
		0x38, 0x44, 0x44, 0x44, 0x38,// o
		0x7C, 0x14, 0x14, 0x14, 0x08,// p
		0x08, 0x14, 0x14, 0x18, 0x7C,// q
		0x7C, 0x08, 0x04, 0x04, 0x08,// r
		0x48, 0x54, 0x54, 0x54, 0x20,// s
		0x04, 0x3F, 0x44, 0x40, 0x20,// t
		0x3C, 0x40, 0x40, 0x20, 0x7C,// u
		0x1C, 0x20, 0x40, 0x20, 0x1C,// v
		0x3C, 0x40, 0x30, 0x40, 0x3C,// w
		0x44, 0x28, 0x10, 0x28, 0x44,// x
		0x0C, 0x50, 0x50, 0x50, 0x3C,// y
		0x44, 0x64, 0x54, 0x4C, 0x44,// z
		0x00, 0x08, 0x36, 0x41, 0x00,// {
		0x00, 0x00, 0x7F, 0x00, 0x00,// |
		0x00, 0x41, 0x36, 0x08, 0x00,// }
		0x08, 0x08, 0x2A, 0x1C, 0x08,// ->
		0x08, 0x1C, 0x2A, 0x08, 0x08 // <-
};


void font_vypis(){

	int k, pom = 0;
	uint8_t i, j;
	uint8_t stop = 100;

	for(i = 0; i < 64; i += 8){

		SetPosition(0, i);

		for(k = pom; k < pom + stop; k += 5){

			for(j = 0; j < 5; j++){

				Glcd_WriteData(font[j + k]);
			}
			Glcd_WriteData(0x00);
		}

		pom += 100;
		if(pom == 400) stop = 80;
		if(pom >= 475) break;
	}
}


/*Funkce pro praci se znaky, znakova sada neni kompletni, jsou zde jen zakladni pismena, cisla a znaky
 *	zacina od hodnoty 32 v ascii tabulce, je tady 96 znaku namapovanych
*/


unsigned char ReadByteFromRom(char * ptr){

	return *(ptr);
}


/*funkci WrChar() predam znak co chci zobrazit, dal ho predam funkci ReadByte..(), ktera ocekava
 * adresu znaku, delam to tak ze adresu pole font[] pretypuju na int, prictu 5nasobek cisla znaku, to je skrz to ze
 * jeden znak odpovida peti hodnotam v poli font[] a iteruju, pak se ta hodnota pretypuje na typ ukazatele na char a predam to
 * funkci ReadByte..(), ktera vrati polozku v poli font[] a to dam jako parametr funkci WrDat()   480
 *
 */
void WriteChar(char charToWrite){

	if(charToWrite >= 126) return;

	uint8_t i;
	charToWrite -= 32;
	uint8_t pom_barva = barva;

	for(i = 0; i < 5; i++){

		if(pom_barva) Ddram_WriteByte(pomX, pomY, ReadByteFromRom((char *)((int)font + (5 * charToWrite) + i)));		//Glcd_WriteData(ReadByteFromRom((char *)((int)font + (5 * charToWrite) + i)));
		else Ddram_WriteByte(pomX, pomY, ~ReadByteFromRom((char *)((int)font + (5 * charToWrite) + i)));		//Glcd_WriteData(~ReadByteFromRom((char *)((int)font + (5 * charToWrite) + i)));

		CursorMove(pomX + 1, pomY);
		if(pomX > 127 || pomY > 63) {
			barva = pom_barva;
			return;
		}
	}

	if (pom_barva)	Ddram_WriteByte(pomX, pomY, 0x00);
	else Ddram_WriteByte(pomX, pomY, 0xff);

	CursorMove(pomX + 1, pomY);

	barva = pom_barva;
}

void Flush(){

	uint8_t poc_x = curX;
	uint8_t kon_x = pomX;
	uint8_t y_pom = pomY;

	for(uint8_t y = curY * 8; y < y_pom + 8; y += 8){

		SetPosition(curX, y);

		for(uint8_t x = poc_x; x < kon_x; x++){

			Glcd_WriteData(DDRAM[y/8][x]);
		}
		curX = poc_x;
	}
}

void CursorMove(uint8_t x, uint8_t y){
	pomX = x;
	pomY = y;
}

void WriteString(char *str){

	while(*str){
		WriteChar(*str++);
		if(pomX > 127 || pomY > 63) return;
	}
}

void BitMapPrint(uint8_t x, uint8_t y, uint16_t *bitmap, uint16_t delka){

	uint8_t col = x;
	uint8_t row = y;
	uint8_t pom_barva = barva;

	for(uint16_t i = 0; i < delka; i++ ){

		if(pom_barva) Ddram_WriteByte(col, row, bitmap[i]);
		else Ddram_WriteByte(col, row, ~bitmap[i]);

		if(++col >= 127){
			col = x;
			row += 8;
			if(row > 63){
				barva = pom_barva;
				return;
			}
			//SetPosition(x, row);
		}
	}
	barva = pom_barva;
}

void Start_screen(){

	FullRectangle(5,1,10,5);
	FullRectangle(5,5,5,10);
	FullRectangle(5,14,5,10);
	FullRectangle(5,23,10,5);

	SetPosition(18, 22);
	WriteString("ORTEX-");
	Flush();
	Vline(55,1,28);
	Line(55, 1, 65, 10);
	Line(65, 10, 75, 1);
	Vline(75,1,28);

	FullCircle(93,16,15);

	FullRectangle(110,16,10,2);
	FullRectangle(114,12,2,10);

	FullRectangle(0,35,127,28);
	SetPosition(25,45);
	SetColor(0);
	WriteString("**UTB Zlin**");
	Flush();

}





