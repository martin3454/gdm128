
#include "MKL25Z4.h"


//cekaci funkce a init

void Wait_Nop (uint32_t cekej);
void Pin_Init();

//4094 FUNKCE SR
void SR_WriteData(uint8_t data);

//GDM LOW LEVEL
void Glcd_Enable_H();
void Glcd_Enable_L();
void Glcd_RS_L();
void Glcd_RS_H();
void Glcd_EnableControler(uint8_t control);
void Glcd_DisableControler(uint8_t control);
void Glcd_WriteCommand(uint8_t cmd, uint8_t control);
void Glcd_WriteData(uint8_t dat);

//kresleni
void SetPosition(uint8_t col, uint8_t row);
void SetPixel(uint8_t x, uint8_t y);
void SetColor(uint8_t col);
void FillScreen();
void ClearScreen();
void Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
void Rectangle(uint8_t x, uint8_t y, uint8_t a, uint8_t b);
void FullRectangle(uint8_t x, uint8_t y, uint8_t a, uint8_t b);
void Vline(uint8_t x1, uint8_t y1, uint8_t y2);
void Hline(uint8_t x1, uint8_t y1, uint8_t x2);
void Circle(uint8_t x, uint8_t y, uint8_t r);
void FullCircle(uint8_t x, uint8_t y, uint8_t r);
void BitMapPrint(uint8_t x, uint8_t y, uint16_t *bitmap, uint16_t delka);
void StartScreen();
void circle_points(uint8_t x, uint8_t y, uint8_t xc, uint8_t yc);


void GlcdInit();
void DdramToLCD(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
void Flush();


//ddram
void Ddram_Clear();
void Ddram_Set();
uint8_t Ddram_Read();
void Ddram_Write(uint8_t x, uint8_t y);
void Ddram_WriteByte(uint8_t x, uint8_t y, uint8_t byte);



//ZNAKOVA SADA
void WriteChar(char charToWrite);
void CursorMove(uint8_t x, uint8_t y);
void WriteString(char* str);
unsigned char ReadByteFromRom(char * ptr);


//pokus
void font_vypis();



