
#include "MKL25Z4.h"


#define M_SW1  (1<<4)
#define M_SW2  (1<<5)
#define M_SW3  (1<<16)
#define M_SW4  (1<<17)

#define NUM_ROWS 3     //ROZMERY POLE PRO POZICE POSTAV
#define NUM_COLS 5
#define GAME_WIDTH 94	//SIRKA HERNI PLOCHY
#define DROP 4

#define RYCHLOST 6
#define CHARACTER_WIDTH 11				//SIRKA NEPRATEL
#define CHARACTER_HEIGH 7				//VYSKA NEPRATEL
#define INVADERS_DROP 4					//SKOK DOLU

#define ACTIVE 0				//OBJEKT SE BUDE ZOBRAZOVAT
#define DESTROYED 2				//OBJEKT SE NEBUDE ZOBRAZOVAT

#define TANK_WIDTH 13
#define TANK_HEIGH 8
#define TANK_STEPX 2
#define TANK_STARTXPOS 63
#define TANK_STARTYPOS 60

#define ROCKET_HEIGH 4
#define ROCKET_WIDTH 1
#define ROCKET_SPEED 4

#define stat1 16
#define stat2 32
#define stat3 48



typedef struct gameObject{
	int x;
	int y;
	uint8_t status;
}GameObject;


typedef struct alienObject{
	GameObject ord;
}AlienObject;


typedef struct tankObject{
	GameObject ord;
}TankObject;



////////////////////////////////
void Enemy_Init();
void Tank_Init();
void Screen_Update();
void Screen_Clear();
void Screen_Delete();


////////////////////////////////vykreslovani
void Tank_Update();
void Tank_Clear();
void Rocket_Update();
void Rocket_Clear();

void Chobot_vykreslit();
void Chobot1_vykreslit();
void Meduza_vykreslit();
void Meduza1_vykreslit();
void Krakatice_vykreslit();
void Krakatice1_vykreslit();

////////////////////////////pozice
int Right_Position();
int Left_Position();

//////////////////////////////ridici logika
void Enemy_Control();
void Tank_Control();
void Rocket_Control();
void Collision_Control();
void Kolize_postava_raketa();
_Bool Kolize(uint8_t alienX, uint8_t alienY);

/////////////////////////////////////////obrazovky
void Game_Stats();
void Game_StatsRefresh();
void GameStart();    ///hlavni smycka


