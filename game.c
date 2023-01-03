
#include "MKL25Z4.h"
#include "game.h"
#include "timer.h"


uint8_t chobot_bitMap[]={

		0b01110000,
		0b00011000,
		0b01111100,
		0b10110110,
		0b00111100,
		0b00111100,
		0b00111100,
		0b10110110,
		0b01111100,
		0b00011000,
		0b01110000
	};

uint8_t chobot1_bitMap[]={

		0b00001110,
		0b10010000,
		0b01111100,
		0b00110110,
		0b00111100,
		0b00111100,
		0b00111100,
		0b00110110,
		0b01111100,
		0b10010000,
		0b00001110

	};

uint8_t meduza_bitMap[]={

		0b10000000,
		0b10110000,
		0b01001000,
		0b11111110,
		0b01111110,
		0b11011110,
		0b01111110,
		0b11111110,
		0b01001000,
		0b10110000,
		0b10000000
};

uint8_t meduza1_bitMap[]={

		0b01000000,
		0b10000000,
		0b01111000,
		0b11111110,
		0b01111110,
		0b11011110,
		0b01111110,
		0b11111110,
		0b01111000,
		0b10000000,
		0b01000000
};


uint8_t krakatice_bitMap[]={

		0b10000000,
		0b10000000,
		0b11011100,
		0b00110100,
		0b11111110,
		0b00111010,
		0b11111110,
		0b00110100,
		0b11011100,
		0b10000000,
		0b10000000
};


uint8_t krakatice1_bitMap[]={

		0b10000000,
		0b10000000,
		0b11011100,
		0b00110100,
		0b11101110,
		0b00101010,
		0b11101110,
		0b00110100,
		0b11011100,
		0b10000000,
		0b10000000
};


uint8_t tank_bitMap[]={

		0b11100000,
		0b11100000,
		0b11100000,
		0b11100000,
		0b11110000,
		0b11111000,
		0b11111111,
		0b11111000,
		0b11110000,
		0b11100000,
		0b11100000,
		0b11100000,
		0b11100000
};

uint8_t raketa_bitMap[]={

		0b11110000
};

uint8_t pozice_ufonuX[NUM_COLS]={4, 19, 34, 49, 64};
uint8_t pozice_ufonuY[NUM_ROWS]={4, 12, 20}; 							//0,9,17
AlienObject ufoni[NUM_ROWS][NUM_COLS];
TankObject hrac;
GameObject raketa;
uint8_t score0, score1, score2;


//char dobaCyklu[10];
//uint32_t out_loop_time = 0;

uint32_t in_loop_time = 0;
int8_t krokX = 2;
int8_t EnemaciCounter = RYCHLOST;
_Bool animace = 0;
_Bool GAME_ON = 1;
int8_t y_f[3] = {4, 12, 20};
int8_t drop = 0;
_Bool vyhra = 0;
_Bool konec = 0;


void GameStart(){									//hlavni smycka

	do{
		if(GAME_ON){

			Enemy_Init();
			Tank_Init();
			Game_Stats();
		}

		while(GAME_ON){

			in_loop_time = Get_Time();

			Screen_Update();

			Enemy_Control();

			Collision_Control();

			while (Get_Time() < in_loop_time + 50);

			Rocket_Clear();

			Rocket_Control();

			Tank_Clear();

			Tank_Control();

			Screen_Clear();

		}

		if(!GAME_ON && konec){
			konec = 0;
			ClearScreen();
			SetPosition(30,10);
			if(vyhra) WriteString("VYHRAL JSI");
			else WriteString("PROHRAL JSI");
			Flush();
			SetPosition(0,32);
			WriteString("Pro restart/konec");
			Flush();
			SetPosition(0,40);
			WriteString("Stiskni SW3/SW4");
			Flush();
			score0 = score1 = score2 = 0;
			vyhra = 0;
		}

		if((PTA->PDIR & M_SW3) == 0) {
			GAME_ON = 1;
			ClearScreen();
			krokX = 2;
			animace = 0;
			drop = 0;
			y_f[0] = 4;
			y_f[1] = 12;
			y_f[2] = 20;
		}

		if((PTA->PDIR & M_SW4) == 0){
			ClearScreen();
			return;
		}
 	}while(1);
}


/*void my_itoa(uint32_t n){

    int delka, i = 0;
    int pom = n;

    for(delka = 0; pom /= 10; delka++);

    delka++;
    i = delka;

    do{
        dobaCyklu[--i] = n % 10 + '0';
    }while(n /= 10);

    dobaCyklu[delka] = '\0';
}*/


void Screen_Clear(){

	uint8_t i, j, sirka;

	if(drop){

		SetPosition(0, y_f[0] - 4);

		for(j = 0; j < GAME_WIDTH; j++){
			Glcd_WriteData(0x00);
		}
	}

	for(i = 0 ; i < 3; i++){

		if(y_f[i] > 0) SetPosition(0, y_f[i]);
		else continue;

		for(j = 0; j < GAME_WIDTH; j++){
			Glcd_WriteData(0x00);
		}
	}

}


void Tank_Control(){

	if((PTA->PDIR & M_SW2) == 0 && hrac.ord.x + TANK_WIDTH < GAME_WIDTH )
		hrac.ord.x += TANK_STEPX;
	if((PTA->PDIR & M_SW1) == 0 && hrac.ord.x > 0)
		hrac.ord.x -= TANK_STEPX;

	//raketa
	if((PTA->PDIR & M_SW3) == 0 & raketa.status != ACTIVE){
		raketa.x = hrac.ord.x + (TANK_WIDTH / 2);
		raketa.y = TANK_STARTYPOS;
		raketa.status = ACTIVE;
	}
}

void Rocket_Control(){

	if(raketa.status == ACTIVE){
		if(raketa.y + ROCKET_HEIGH <= 0)
			raketa.status = DESTROYED;
		else raketa.y -= ROCKET_SPEED;
	}
}

void Collision_Control(){

	Kolize_postava_raketa();
}

void Kolize_postava_raketa(){

	uint8_t j, i;

	if(raketa.status == ACTIVE){

		for(i = 0; i < NUM_ROWS; i++){

			if(raketa.status == DESTROYED) break;

			for(j = 0; j < NUM_COLS; j++){

				if(ufoni[i][j].ord.status == ACTIVE){

					if(Kolize(ufoni[i][j].ord.x, ufoni[i][j].ord.y)){				//zasah

						ufoni[i][j].ord.status = DESTROYED;

						switch(i){

							case 0: score0++;
							break;

							case 1: score1++;
							break;

							case 2: score2++;
							break;
						}

						Game_StatsRefresh();

						if(score0 + score1 + score2 == 15) {
							GAME_ON = 0;
							vyhra = 1;
							konec = 1;
						}

						raketa.status = DESTROYED;
						break;
					}
				}
			}
		}
	}


}


void Enemy_Control(){

	if((EnemaciCounter--) < 0){

		drop = 0;

		if(Right_Position() + krokX >= GAME_WIDTH || Left_Position() + krokX <= 0){

			krokX *= -1;

			drop = 1;

			if(score0 == 5) y_f[0] = -1;
			else y_f[0] += 4;

			if(score1 == 5) y_f[1] = -1;
			else y_f[1] += 4;

			if(score2 == 5) y_f[2] = -1;
			else y_f[2] += 4;
		}

		uint8_t j, i;

		for(i = 0; i < NUM_ROWS; i++){

			for(j = 0; j < NUM_COLS; j++){

				if(ufoni[i][j].ord.status == ACTIVE){

					if(drop == 0){

						ufoni[i][j].ord.x += krokX;
					}else {

						ufoni[i][j].ord.y += DROP;
						uint8_t al_y = ufoni[i][j].ord.y;
						uint8_t poloha = hrac.ord.y - 5;


						if(al_y >= poloha) {
							GAME_ON = 0;
							konec = 1;
							vyhra = 0;
						}
					}
				}
			}
		}
		EnemaciCounter = RYCHLOST;
		//animace = !animace;
	}
}


void Screen_Update(){

	Tank_Update();

	if(raketa.status == ACTIVE) Rocket_Update();

	uint8_t j, i;

	for(j = 0; j < NUM_ROWS; j++){

		for(i = 0; i < NUM_COLS ; i++){

			if(ufoni[j][i].ord.status == ACTIVE){

				SetPosition(ufoni[j][i].ord.x, ufoni[j][i].ord.y );

				switch(j){

						case 0:

								if(animace) Chobot_vykreslit();
								else Chobot1_vykreslit();

						break;

						case 1:

								if(animace) Meduza_vykreslit();
								else Meduza1_vykreslit();

						break;

						case 2:

								if(animace) Krakatice_vykreslit();
								else Krakatice1_vykreslit();
						break;
			  }
		  }

		}
	}
}

void Enemy_Init(){

	uint8_t j, i;

	for(i = 0; i < NUM_ROWS; i++){

		for(j = 0; j < NUM_COLS; j++){

			ufoni[i][j].ord.x = pozice_ufonuX[j];
			ufoni[i][j].ord.y = pozice_ufonuY[i];
			ufoni[i][j].ord.status = ACTIVE;

		}
	}
}

void Chobot_vykreslit(){

	for(uint8_t x = 0; x < CHARACTER_WIDTH; x++)
		Glcd_WriteData(chobot_bitMap[x]);
}

void Chobot1_vykreslit(){

	for(uint8_t x = 0; x < CHARACTER_WIDTH; x++)
		Glcd_WriteData(chobot1_bitMap[x]);
}

void Tank_Update(){

	SetPosition(hrac.ord.x, hrac.ord.y );
	for(uint8_t x = 0; x < TANK_WIDTH; x++)
		Glcd_WriteData(tank_bitMap[x]);
}

void Tank_Clear(){

	SetPosition(hrac.ord.x, hrac.ord.y );
	for(uint8_t x = 0; x < TANK_WIDTH; x++)
		Glcd_WriteData(0x00);
}

void Rocket_Update(){

	SetPosition(raketa.x, raketa.y );
	Glcd_WriteData(raketa_bitMap[0]);
}

void Rocket_Clear(){

	SetPosition(raketa.x, raketa.y );
	Glcd_WriteData(0x00);
}

void Meduza_vykreslit(){

	for(uint8_t x = 0; x < CHARACTER_WIDTH; x++)
		Glcd_WriteData(meduza_bitMap[x]);
}

void Meduza1_vykreslit(){

	for(uint8_t x = 0; x < CHARACTER_WIDTH; x++)
		Glcd_WriteData(meduza1_bitMap[x]);
}

void Krakatice_vykreslit(){

	for(uint8_t x = 0; x < CHARACTER_WIDTH; x++)
		Glcd_WriteData(krakatice_bitMap[x]);
}

void Krakatice1_vykreslit(){

	for(uint8_t x = 0; x < CHARACTER_WIDTH; x++)
		Glcd_WriteData(krakatice1_bitMap[x]);
}


int Left_Position(){

	int i,down, nejmensi, leva_pos;
	nejmensi = 200;

	for(i = 0; i < NUM_COLS; i++){

		down = 0;

		while(down < NUM_ROWS){

			if(ufoni[down][i].ord.status == ACTIVE){

				leva_pos = ufoni[down][i].ord.x;
				if(leva_pos < nejmensi)

					nejmensi = leva_pos;
			}
			down++;
		}
		if(nejmensi < 200) return nejmensi;
	}
	return 0;
}

int Right_Position(){

	int i,down, nejvetsi, prava_pos;
	nejvetsi = 0;

	for(i = NUM_COLS - 1; i >= 0; i--){

		down = 0;

		while(down < NUM_ROWS){

			if(ufoni[down][i].ord.status == ACTIVE){

				prava_pos = ufoni[down][i].ord.x + CHARACTER_WIDTH;

				if(prava_pos > nejvetsi)
					nejvetsi = prava_pos;
			}
			down++;
		}
		if(nejvetsi > 0) return nejvetsi;
	}
	return 0;
}

_Bool Kolize(uint8_t alienX, uint8_t alienY ){
	return ((raketa.x  > alienX) & (raketa.x < alienX + CHARACTER_WIDTH) & (raketa.y + ROCKET_HEIGH > alienY) & (raketa.y < alienY + CHARACTER_HEIGH ));
}

void Tank_Init(){

	PORTA->PCR[4] = 0x100;
	PORTA->PCR[5] = 0x100;
	PORTA->PCR[16] = 0x100;
	PORTA->PCR[17] = 0x100;

	PTA->PDDR &= ~(1<<4);
	PTA->PDDR &= ~(1<<5);
	PTA->PDDR &= ~(1<<16);
	PTA->PDDR &= ~(1<<17);

	hrac.ord.x = TANK_STARTXPOS;
	hrac.ord.y = TANK_STARTYPOS;

	raketa.status = DESTROYED;

	score0 = score1 = score2 = 0;

}

void Game_Stats(){

	Vline(GAME_WIDTH + 1, 1, 62);

	FullRectangle(96,1,30,10);

	SetPosition(99,3);
	SetColor(0);
	WriteString("body");
	Flush();

	SetColor(1);
	Vline(126,1,62);

	SetPosition(102,stat1);
	Chobot_vykreslit();

	SetPosition(114,stat1);
	WriteString(" ");
	Flush();

	SetPosition(120,stat1);
	WriteChar((char)score0+'0');
	Flush();

	SetPosition(102,stat2);
	Meduza_vykreslit();

	SetPosition(114,stat2);
	WriteString(" ");
	Flush();

	SetPosition(120,stat2);
	WriteChar((char)score1+'0');
	Flush();

	SetPosition(102,stat3);
	Krakatice_vykreslit();

	SetPosition(114,stat3);
	WriteString(" ", 1);
	Flush();

	SetPosition(120,stat3);
	WriteChar((char)score2+'0');
	Flush();



}

void Game_StatsRefresh(){

	uint8_t i, j;

	for(i = stat1; i < 64; i += 8){

		SetPosition(120, i);

		for(j = 0; j < 5; j++){
			Glcd_WriteData(0x00);
		}
	}

	/*SetPosition(105, stat1);
	WriteString(dobaCyklu);
	Flush();*/

	SetPosition(120,stat1);
	WriteChar((char)score0+'0');
	Flush();

	SetPosition(120,stat2);
	WriteChar((char)score1+'0');
	Flush();

	SetPosition(120,stat3);
	WriteChar((char)score2+'0');
	Flush();

}
