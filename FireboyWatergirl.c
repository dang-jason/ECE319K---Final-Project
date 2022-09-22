// FireboyWatergirl.c
// Runs on TM4C123
// Gabriel Mi & Jason Dang
// EE319K Lab 10

// Last Modified: 5/3/2022 
// https://fireboyand-watergirl.co/
// sounds at https://downloads.khinsider.com/game-soundtracks/album/fireboy-and-watergirl
// https://www.fireboynwatergirl.com/?p=walkthrough&id=1

// ******* Hardware I/O connections *******************
// Slide pot 1 pin 1 connected to ground
// Slide pot 1 pin 2 connected to PE4/Ain9
// Slide pot 1 pin 3 connected to +3.3V 
// Slide pot 2 pin 1 connected to ground
// Slide pot 2 pin 2 connected to PE5/Ain8
// Slide pot 2 pin 3 connected to +3.3V 
// Buttons connected to PE0-PE1
// 32*R resistor DAC bit 0 on PB0 (least significant bit)
// 16*R resistor DAC bit 1 on PB1
// 8*R resistor DAC bit 2 on PB2 
// 4*R resistor DAC bit 3 on PB3
// 2*R resistor DAC bit 4 on PB4
// 1*R resistor DAC bit 5 on PB5 (most significant bit)


#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "Print.h"
#include "TExaS.h"
#include "ADC.h"
#include "Images.h"
#include "Sound.h"
#include "Timer0.h"
#include "PLL.h"
#include "Move.h"
#define  w   12
#define  h   18
#define  dx   2


void DisableInterrupts(void);    // Disable interrupts
void EnableInterrupts(void);     // Enable interrupts


typedef enum {alive,dead} status_t;
typedef enum {starting,playing,ending} game_t;
typedef enum {English,Spanish} language_t;

typedef struct player{
	uint8_t x;
	uint8_t y;
	uint8_t xold;
	uint8_t yold;
	uint8_t moved;
	uint8_t jumped;
	uint8_t jumped2;
}player_t;
player_t fireboy = {4,123,4,123,0,0,0};
player_t watergirl = {4,102,4,102,0,0,0};

uint8_t redgems[4];
uint8_t bluegems[4];
uint8_t platforms[2];
uint16_t score = 0;
status_t status;
game_t game = starting;
language_t language = English;
char fireboystring[8] = {'F','i','r','e','b','o','y',0};
char watergirlstring[10] = {'W','a','t','e','r','g','i','r','l',0};

void Game_Init(void){
	fireboy.x = 4;
	watergirl.x = 4;
	fireboy.y = 123;
	watergirl.y = 102;
	fireboy.jumped = 0;
	watergirl.jumped = 0;
	for(uint8_t i = 0; i < 4; i++){
		redgems[i] = 0;
		bluegems[i] = 0;
	}
	platforms[0] = 0;
	platforms[1] = 0;
	score = 0;
	status = alive;
	ST7735_DrawBitmap(0,127,level1,160,128);   //level
	ST7735_DrawBitmap(4,26,enddoor,18,22);     //enddoor
  ST7735_DrawBitmap(4,55,platform,19,4);     //platform1
  ST7735_DrawBitmap(138,30,platform,19,4);   //platform2
  ST7735_DrawBitmap(80,123,redgem,8,8);      //redgem1
	ST7735_DrawBitmap(58,81,redgem,8,8);       //redgem2
  ST7735_DrawBitmap(50,51,redgem,8,8);       //redgem3
	ST7735_DrawBitmap(33,26,redgem,8,8);       //redgem4
  ST7735_DrawBitmap(113,123,bluegem,8,8);    //bluegem1 
	ST7735_DrawBitmap(90,90,bluegem,8,8);      //bluegem2
	ST7735_DrawBitmap(100,56,bluegem,8,8);     //bluegem3
  ST7735_DrawBitmap(90,26,bluegem,8,8);      //bluegem4
	ST7735_DrawBitmap(fireboy.x,fireboy.y,boy,w,h);
	ST7735_DrawBitmap(watergirl.x,watergirl.y,girl,w,h);
	if(language == English){
		ST7735_SetCursor(9,0);
		ST7735_OutString("Score:");
	}
	else if(language == Spanish){
		ST7735_SetCursor(9,0);
		ST7735_OutString("Puntaje:");		
	}
}


void gems(void){
	if(fireboy.x >= 76 && fireboy.x <= 80 && fireboy.y == 123 && redgems[0] == 0){
		score += 125;
		redgems[0] = 1;
		Sound_gem();
	}
	else if(fireboy.x >= 54 && fireboy.x <= 58 && fireboy.y == 81 && redgems[1] == 0){
		score += 125;
		redgems[1] = 1;
		Sound_gem();
	}
	else if(fireboy.x >= 46 && fireboy.x <= 50 && fireboy.y == 51 && redgems[2] == 0){
		score += 125;
		redgems[2] = 1;
		Sound_gem();
	}
	else if(fireboy.x >= 29 && fireboy.x <= 33 && fireboy.y == 26 && redgems[3] == 0){
		score += 125;
		redgems[3] = 1;
		Sound_gem();
	}
	if(watergirl.x >= 109 && watergirl.x <= 113 && watergirl.y == 123 && bluegems[0] == 0){
		score += 125;
		bluegems[0] = 1;
		Sound_gem();
	}
	else if(watergirl.x >= 86 && watergirl.x <= 90 && watergirl.y == 90 && bluegems[1] == 0){
		score += 125;
		bluegems[1] = 1;
		Sound_gem();
	}
	else if(watergirl.x >= 96 && watergirl.x <= 100 && watergirl.y == 56 && bluegems[2] == 0){
		score += 125;
		bluegems[2] = 1;
		Sound_gem();
	}
	else if(watergirl.x >= 86 && watergirl.x <= 90 && watergirl.y == 26 && bluegems[3] == 0){
		score += 125;
		bluegems[3] = 1;
		Sound_gem();
	}
	if(fireboy.x > 100 && fireboy.x < 122 && fireboy.y < 124 && fireboy.y > 93 && bluegems[0] != 1)
		bluegems[0] = 2;
	else if(fireboy.x > 77 && fireboy.x < 99 && fireboy.y < 91 && fireboy.y > 60 && bluegems[1] != 1)
		bluegems[1] = 2;
	else if(fireboy.x > 87 && fireboy.x < 109 && fireboy.y < 57 && fireboy.y > 30 && bluegems[2] != 1)
		bluegems[2] = 2;
	else if(fireboy.x > 77 && fireboy.x < 99 && fireboy.y < 27 && fireboy.y > 18 && bluegems[3] != 1)
		bluegems[3] = 2;
	if(watergirl.x > 67 && watergirl.x < 89 && watergirl.y < 124 && watergirl.y > 93 && redgems[0] != 1)
		redgems[0] = 2;
	else if(watergirl.x > 45 && watergirl.x < 67 && watergirl.y < 82 && watergirl.y > 73 && redgems[1] != 1)
		redgems[1] = 2;
	else if(watergirl.x > 37 && watergirl.x < 59 && watergirl.y < 53 && watergirl.y > 44 && redgems[2] != 1)
		redgems[2] = 2;
	else if(watergirl.x > 20 && watergirl.x < 42 && watergirl.y < 27 && watergirl.y > 18 && redgems[3] != 1)
		redgems[3] = 2;
}


void checkgem(void){
	if(bluegems[0] == 2){
		ST7735_DrawBitmap(113,123,bluegem,8,8);
		bluegems[0] = 0;
	}
	else if(bluegems[1] == 2){
		ST7735_DrawBitmap(90,90,bluegem,8,8);
		bluegems[1] = 0;
	}
	else if(bluegems[2] == 2){
		ST7735_DrawBitmap(100,56,bluegem,8,8);
		bluegems[2] = 0;
	}
	else if(bluegems[3] == 2){
		ST7735_DrawBitmap(90,26,bluegem,8,8);
		bluegems[3] = 0;
	}
	if(redgems[0] == 2){
		ST7735_DrawBitmap(80,123,redgem,8,8);
		redgems[0] = 0;
	}
	else if(redgems[1] == 2){
		ST7735_DrawBitmap(58,81,redgem,8,8);
		redgems[1] = 0;
	}
	else if(redgems[2] == 2){
		ST7735_DrawBitmap(50,51,redgem,8,8);
		redgems[2] = 0;
	}
	else if(redgems[3] == 2){
		ST7735_DrawBitmap(33,26,redgem,8,8);
		redgems[3] = 0;
	}
}


void puddles(void){
	if(fireboy.x > (109 - w/2) && fireboy.x < (125 - w/2) && fireboy.y == 123){
		status = dead;
		Sound_dying();
	}
	if(watergirl.x > (75 - w/2) && watergirl.x < (93 - w/2) && watergirl.y == 123){
		status = dead;
		Sound_dying();
	}
	if(fireboy.x > (99 - w/2) && fireboy.x < (117 - w/2) && fireboy.y == 90){
		status = dead;
		Sound_dying();
	}
	if(watergirl.x > (99 - w/2) && watergirl.x < (117 - w/2) && watergirl.y == 90){
		status = dead;
		Sound_dying();
	}
	if(fireboy.x > (113 - w/2) && fireboy.x < (127 - w/2) && fireboy.y == 26){
		status = dead;
		Sound_dying();
	}
	if(watergirl.x > (113 - w/2) && watergirl.x < (127 - w/2) && watergirl.y == 26){
		status = dead;
		Sound_dying();
	}
}


void checkplatform(void){
	if(fireboy.x > 29 && fireboy.x < 37 && fireboy.y == 81 && watergirl.x > 29 && watergirl.x < 37 && watergirl.y == 81 && platforms[0] == 0){
		platforms[0] = 1;
		Sound_platmove();
	}
	else if(platforms[0] != 2 && !(fireboy.x > 29 && fireboy.x < 37 && fireboy.y == 81 && watergirl.x > 29 && watergirl.x < 37 && watergirl.y == 81))
		platforms[0] = 0;
	if(fireboy.x > 109 && fireboy.x < 117 && fireboy.y == 56 && watergirl.x > 109 && watergirl.x < 117 && watergirl.y == 56 && platforms[1] == 0){
		platforms[1] = 1;
		Sound_platmove();
	}
	else if(platforms[1] != 2 && !(fireboy.x > 109 && fireboy.x < 117 && fireboy.y == 56 && watergirl.x > 109 && watergirl.x < 117 && watergirl.y == 56))
		platforms[1] = 0;
}


void moveplatform(void){
	static uint8_t i = 0;
	if(platforms[0] == 1){
		for(int i = 0; i < 500000; i++){}
		ST7735_FillRect(4,51+i,19,4,0x1186);
		ST7735_DrawBitmap(4,55+i,platform,19,4);
		i = (i + 1) % 31;
		if(i == 0)
			platforms[0] = 2;
	}
	else if(platforms[1] == 1){
		for(int i = 0; i < 500000; i++){}
		ST7735_FillRect(138,26+i,19,4,0x1186);
		ST7735_DrawBitmap(138,30+i,platform,19,4);
		i = (i + 1) % 31;
		if(i == 0)
			platforms[1] = 2;
	}
}


void move(void){
	static uint8_t i = 0, j = 0;
	int8_t jumparr[19] = {-2,-5, -7, -6, -4, -3, -2, -2, -2, 0, 2, 2, 3, 4, 6, 7, 5, 2, 2};
	int8_t jumparr2[11] = {-2, -4, -3, -2, -1, 0, 1, 2, 3, 4, 2};
  fireboy.xold = fireboy.x;
  fireboy.yold = fireboy.y;
  watergirl.xold = watergirl.x;
  watergirl.yold = watergirl.y;
	uint16_t ADCValue[2];
  ADC_In(ADCValue);
  if(ADCValue[0] < 1365){
		fireboy.moved = 1;
		if(fireboy.x > 4){
			if(fireboy.x == 54 && fireboy.y < 123 && fireboy.y > 102)
				fireboy.x = fireboy.x;
			else if(fireboy.x == 134 && fireboy.y < 111 && fireboy.y > 90)
				fireboy.x = fireboy.x;
			else if(fireboy.x == 80 && fireboy.y < 91 && fireboy.y > 81)
				fireboy.x = fireboy.x;
			else if(fireboy.x == 84 && fireboy.y < 57 && fireboy.y > 51)
				fireboy.x = fireboy.x;
			else if(fireboy.x == 138 && fireboy.y < 52 && fireboy.y > 26)
				fireboy.x = fireboy.x;
			else if(fireboy.x == 24 && fireboy.y < 82 && fireboy.y > 56 && platforms[0] != 2)
				fireboy.x = fireboy.x;
			else 
				fireboy.x -= dx;
		}
	}
	else if(ADCValue[0] > 2731){
		fireboy.moved = 1;
		if(fireboy.x < (156 - w)){
			if(fireboy.x == (144 - w) && fireboy.y < 124 && fireboy.y > 110)
				fireboy.x = fireboy.x;
			else if(fireboy.x == (78 - w) && (fireboy.y - h) < 61 && (fireboy.y - h) > 55) 
				fireboy.x = fireboy.x;
			else if(fireboy.x == (118 - w) && (fireboy.y - h + 1) > 29 && fireboy.y < (34 + h))
				fireboy.x = fireboy.x;
			else if(fireboy.x == (22 - w) && fireboy.y < 72 && fireboy.y > 51) //
				fireboy.x = fireboy.x;
			else if(fireboy.x == (138 - w) && fireboy.y < 57 && fireboy.y > 34 && platforms[1] != 2)
				fireboy.x = fireboy.x;
			else
				fireboy.x += dx; 
		}
	}
	if(fireboy.jumped){
		if(fireboy.x < 54 && (fireboy.y + jumparr2[i]) < (105 + h) && fireboy.y > 105){
			fireboy.y = 105 + h;
			fireboy.jumped = 0;
			fireboy.jumped2 = 1;
		}
		else if(fireboy.x > 68 && fireboy.x < 134 && (fireboy.y + jumparr2[i]) < (92 + h) && fireboy.y > 93){
			fireboy.y = 93 + h;
			fireboy.jumped = 0;
			fireboy.jumped2 = 1;
		}
		else if(fireboy.x > 79 && fireboy.x < 134 && (fireboy.y + jumparr2[i]) < (60 + h) && fireboy.y > 60 && fireboy.y < 90){
			fireboy.y = 60 + h;
			fireboy.jumped = 0;
			fireboy.jumped2 = 1;
		}
		else if(fireboy.x > 77 && fireboy.x < 80 && (fireboy.y + jumparr2[i]) < (60 + h) && fireboy.y > 60 && fireboy.y < 81){
			fireboy.y = 60 + h;
			fireboy.jumped = 0;
			fireboy.jumped2 = 1;
		}
		else if(fireboy.x > (22 - w) && fireboy.x < 78 && (fireboy.y + jumparr2[i]) < (55 + h) && fireboy.y > 55 && fireboy.y < 81){
			fireboy.y = 55 + h;
			fireboy.jumped = 0;
			fireboy.jumped2 = 1; 
		}
		else if(fireboy.x < 83 && fireboy.x > 22 && (fireboy.y + jumparr2[i]) < (30 + h) && fireboy.y > 30 && fireboy.y < 51){
			fireboy.y = 30 + h;
			fireboy.jumped = 0;
			fireboy.jumped2 = 1;
		}
		else if(fireboy.x > 82 && fireboy.x < 119 && (fireboy.y + jumparr2[i]) < (30 + h) && fireboy.y < 56 && fireboy.y > 30){
			fireboy.y = 30 + h;
			fireboy.jumped = 0;
			fireboy.jumped2 = 1;
		}
		else if(fireboy.x > 118 && fireboy.x < 138 && (fireboy.y + jumparr2[i]) < (34 + h) && fireboy.y > 34 && fireboy.y < 56){
			fireboy.y = 34 + h;
			fireboy.jumped = 0;
			fireboy.jumped2 = 1;
		}
		else if(fireboy.x > 3 && fireboy.x < 63 && (fireboy.y + jumparr2[i]) < (3 + h) && fireboy.y < 27){
			fireboy.y = 8 + h;
			fireboy.jumped = 0;
			fireboy.jumped2 = 1; 
		}
		else if(fireboy.x > 62 && fireboy.x < 103 && (fireboy.y + jumparr2[i]) < (8 + h) && fireboy.y < 27){
			fireboy.y = 8 + h;
			fireboy.jumped = 0;
			fireboy.jumped2 = 1;
		}
		else if(fireboy.x < (144 - w) && fireboy.x > 22 && (fireboy.y + jumparr2[i]) < 124){
			fireboy.y += jumparr2[i];
			i = (i + 1) % 11;
			fireboy.jumped2 = 1;
			if(i == 0)
				fireboy.jumped = 0;
		}
	}
	if(fireboy.jumped){
		if(fireboy.x < 134 && fireboy.x > 126 && fireboy.y < 91 && fireboy.y > 86 && jumparr2[i] > 0)
			fireboy.jumped = 0;
		else if(fireboy.x < 80 && fireboy.x > 72 && fireboy.y < 82 && fireboy.y > 77 && jumparr2[i] > 0)
			fireboy.jumped = 0;
		else if(fireboy.x > (23 - w) && fireboy.x < 24 && fireboy.y < 52 && fireboy.y > 47 && (jumparr[i] > 0 || jumparr2[i] > 0)) 
			fireboy.jumped = 0;
		else if(fireboy.x < 83 && fireboy.x > 75 && fireboy.y < 52 && fireboy.y > 47 && jumparr2[i] > 0)
			fireboy.jumped = 0;
		else if(fireboy.x < 138 && fireboy.x > 130 && fireboy.y < 27 && fireboy.y > 22 && (jumparr[i] > 0 || jumparr2[i] > 0)) 
			fireboy.jumped = 0;
		else if(fireboy.x > (145 - w) && fireboy.x < 145 && fireboy.y < 112 && fireboy.y > 107 && jumparr[i] > 0)
			fireboy.jumped = 0;
	}
	if(!fireboy.jumped){
		i = 0;
		if(fireboy.x < 134 && fireboy.y > (92 + h) && fireboy.y < 123)
			fireboy.y = 123;
		else if(fireboy.x > 79 && fireboy.x < 136 && fireboy.y > (59 + h) && fireboy.y < 90)
			fireboy.y = 90;
		else if(fireboy.x > 77 && fireboy.x < 80 && fireboy.y > (59 + h) && fireboy.y < 81)
			fireboy.y = 81;
		else if(fireboy.x < 78 && fireboy.x > (23 - w) && fireboy.y > (54 + h) && fireboy.y < 81)
			fireboy.y = 81;
		else if(fireboy.x < 83 && fireboy.x > (24 - w) && fireboy.y > (29 + h) && fireboy.y < 55)
			fireboy.y = 51;
		else if(fireboy.x > 82 && fireboy.x < 119 && fireboy.y > (29 + h) && fireboy.y < 56)
			fireboy.y = 56;
		else if(fireboy.x > 118 && fireboy.x < 138 && fireboy.y > (33 + h) && fireboy.y < 56)
			fireboy.y = 56;
		else if(fireboy.x < 138 && fireboy.y > (1 + h) && fireboy.y < 34)
			fireboy.y = 26;
	}
	if(fireboy.jumped && !fireboy.jumped2){
		if(fireboy.x > 133 && (fireboy.y + jumparr[i]) < (60 + h) && fireboy.y < 110 && fireboy.y > 60){
			fireboy.y = 60 + h;
			fireboy.jumped = 0;
		}
		else if(fireboy.x < 23 && (fireboy.y + jumparr[i]) < (29 + h) && fireboy.y < 81 && fireboy.y > 29){
			fireboy.y = 29 + h;
			fireboy.jumped = 0;
		}
		else if(fireboy.x > 137 && (fireboy.y + jumparr[i]) < (2 + h) && fireboy.y > 56){
			fireboy.y = 2 + h;
			fireboy.jumped = 0;
		}
		else if(fireboy.x > 143 && (fireboy.y + jumparr[i]) > 111 && fireboy.y > 60){
			fireboy.y = 60 + h;
			fireboy.jumped = 0;
		}
		else if(((fireboy.x > (142 - w)) || fireboy.x < 23) && (fireboy.y + jumparr[i]) < 124){
			fireboy.y += jumparr[i];
			i = (i + 1) % 19;
			if(i == 0)
				fireboy.jumped = 0;
		}
		fireboy.moved = 1;
	}
	if(fireboy.jumped){
		if(fireboy.x > (144-w) && fireboy.y < 124 && fireboy.y > 107 && jumparr[i] > 0)
			fireboy.jumped = 0;
	}
	if(!fireboy.jumped){
		i = 0;
		if(fireboy.x > 137 && (fireboy.y < 56) && fireboy.y > 2)
			fireboy.y = 56;
		else if(fireboy.x < (23 - w) && fireboy.y < 81 && fireboy.y > 29)
			fireboy.y = 81;
		else if(fireboy.x > (144 - w) && fireboy.y > 60 && fireboy.y < 124)
			fireboy.y = 111;
	}
	fireboy.jumped2 = 0;
	if(ADCValue[1] < 1365){
		watergirl.moved = 1;
		if(watergirl.x > 4){
			if(watergirl.x == 54 && watergirl.y < 123 && watergirl.y > 102)
				watergirl.x = watergirl.x;
			else if(watergirl.x == 134 && watergirl.y < 111 && watergirl.y > 90)
				watergirl.x = watergirl.x;
			else if(watergirl.x == 80 && watergirl.y < 91 && watergirl.y > 81)
				watergirl.x = watergirl.x;
			else if(watergirl.x == 84 && watergirl.y < 57 && watergirl.y > 51)
				watergirl.x = watergirl.x;
			else if(watergirl.x == 138 && watergirl.y < 52 && watergirl.y > 26)
				watergirl.x = watergirl.x;
			else if(watergirl.x == 24 && watergirl.y < 82 && watergirl.y > 56 && platforms[0] != 2)
				watergirl.x = watergirl.x;
			else 
				watergirl.x -= dx;
		}
	}
	else if(ADCValue[1] > 2731){
		watergirl.moved = 1;
		if(watergirl.x < (156 - w)){
			if(watergirl.x == (144 - w) && watergirl.y < 124 && watergirl.y > 110)
				watergirl.x = watergirl.x;
			else if(watergirl.x == (78 - w) && (watergirl.y - h) < 61 && (watergirl.y - h) > 55) 
				watergirl.x = watergirl.x;
			else if(watergirl.x == (118 - w) && (watergirl.y - h + 1) > 29 && watergirl.y < (34 + h))
				watergirl.x = watergirl.x;
			else if(watergirl.x == (22 - w) && watergirl.y < 72 && watergirl.y > 51)
				watergirl.x = watergirl.x;
			else if(watergirl.x == (138 - w) && watergirl.y < 57 && watergirl.y > 34 && platforms[1] != 2)
				watergirl.x = watergirl.x;
			else
				watergirl.x += dx; 
		}
	}
	if(watergirl.jumped){
		if(watergirl.x < 54 && (watergirl.y + jumparr2[j]) < (105 + h) && watergirl.y > 105){
			watergirl.y = 105 + h;
			watergirl.jumped = 0;
			watergirl.jumped2 = 1;
		}
		else if(watergirl.x < 68 && (watergirl.y + jumparr2[j]) < (84 + h) && watergirl.y > 84 && watergirl.y < 103){  
			watergirl.y = 84 + h;
			watergirl.jumped = 0;
			watergirl.jumped2 = 1;
		}
		else if(watergirl.x > 68 && watergirl.x < 134 && (watergirl.y + jumparr2[j]) < (92 + h) && watergirl.y > 93){
			watergirl.y = 93 + h;
			watergirl.jumped = 0;
			watergirl.jumped2 = 1;
		}
		else if(watergirl.x > 79 && watergirl.x < 134 && (watergirl.y + jumparr2[j]) < (60 + h) && watergirl.y > 60 && watergirl.y < 90){
			watergirl.y = 60 + h;
			watergirl.jumped = 0;
			watergirl.jumped2 = 1;
		}
		else if(watergirl.x > 77 && watergirl.x < 80 && (watergirl.y + jumparr2[j]) < (60 + h) && watergirl.y > 60 && watergirl.y < 81){
			watergirl.y = 60 + h;
			watergirl.jumped = 0;
			watergirl.jumped2 = 1;
		}
		else if(watergirl.x > (22 - w) && watergirl.x < 78 && (watergirl.y + jumparr2[j]) < (55 + h) && watergirl.y > 55 && watergirl.y < 81){
			watergirl.y = 55 + h;
			watergirl.jumped = 0;
			watergirl.jumped2 = 1;
		}
		else if(watergirl.x < 83 && watergirl.x > 22 && (watergirl.y + jumparr2[j]) < (30 + h) && watergirl.y > 30 && watergirl.y < 51){
			watergirl.y = 30 + h;
			watergirl.jumped = 0;
			watergirl.jumped2 = 1;
		}
		else if(watergirl.x > 82 && watergirl.x < 119 && (watergirl.y + jumparr2[j]) < (30 + h) && watergirl.y < 56 && watergirl.y > 30){
			watergirl.y = 30 + h;
			watergirl.jumped = 0;
			watergirl.jumped2 = 1;
		}
		else if(watergirl.x > 118 && watergirl.x < 138 && (watergirl.y + jumparr2[j]) < (34 + h) && watergirl.y > 34 && watergirl.y < 56){
			watergirl.y = 34 + h;
			watergirl.jumped = 0;
			watergirl.jumped2 = 1;
		}
		else if(watergirl.x > 3 && watergirl.x < 63 && (watergirl.y + jumparr2[j]) < (3 + h) && watergirl.y < 27){
			watergirl.y = 8 + h;
			watergirl.jumped = 0;
			watergirl.jumped2 = 1;
		}
		else if(watergirl.x > 62 && watergirl.x < 103 && (watergirl.y + jumparr2[j]) < (8 + h) && watergirl.y < 27){
			watergirl.y = 8 + h;
			watergirl.jumped = 0;
			watergirl.jumped2 = 1;
		}
		else if(watergirl.x < (144 - w) && watergirl.x > 22 && (watergirl.y + jumparr2[j]) < 124){
			watergirl.y += jumparr2[j];
			j = (j + 1) % 11;
			watergirl.jumped2 = 1;
			if(j == 0)
				watergirl.jumped = 0;
		}
	}
	if(watergirl.jumped){
		if(watergirl.x < 134 && watergirl.x > 126 && watergirl.y < 91 && watergirl.y > 86 && jumparr2[j] > 0)
			watergirl.jumped = 0;
		else if(watergirl.x < 80 && watergirl.x > 72 && watergirl.y < 82 && watergirl.y > 77 && jumparr2[j] > 0)
			watergirl.jumped = 0;
		else if(watergirl.x > (23 - w) && watergirl.x < 24 && watergirl.y < 52 && watergirl.y > 47 && (jumparr[j] > 0 || jumparr2[j] > 0)) 
			watergirl.jumped = 0;
		else if(watergirl.x < 83 && watergirl.x > 75 && watergirl.y < 52 && watergirl.y > 47 && jumparr2[j] > 0)
			watergirl.jumped = 0;
		else if(watergirl.x < 138 && watergirl.x > 130 && watergirl.y < 27 && watergirl.y > 22 && (jumparr[j] > 0 || jumparr2[j] > 0)) 
			watergirl.jumped = 0;
		else if(watergirl.x > (145 - w) && watergirl.x < 145 && watergirl.y < 112 && watergirl.y > 107 && jumparr[j] > 0)
			watergirl.jumped = 0;
	}
	if(!watergirl.jumped){
		j = 0;
		if(watergirl.x > 53 && watergirl.x < (70 - w) && watergirl.y > (83 + h) && watergirl.y < 123)
			watergirl.y = 123;
		else if(watergirl.x < 134 && watergirl.y > (92 + h) && watergirl.y < 123)
			watergirl.y = 123;
		else if(watergirl.x > 79 && watergirl.x < 136 && watergirl.y > (59 + h) && watergirl.y < 90)
			watergirl.y = 90;
		else if(watergirl.x > 77 && watergirl.x < 80 && watergirl.y > (59 + h) && watergirl.y < 81)
			watergirl.y = 81;
		else if(watergirl.x < 78 && watergirl.x > (23 - w) && watergirl.y > (54 + h) && watergirl.y < 81)
			watergirl.y = 81;
		else if(watergirl.x < 83 && watergirl.x > (24 - w) && watergirl.y > (29 + h) && watergirl.y < 55)
			watergirl.y = 51;
		else if(watergirl.x > 82 && watergirl.x < 119 && watergirl.y > (29 + h) && watergirl.y < 56)
			watergirl.y = 56;
		else if(watergirl.x > 118 && watergirl.x < 138 && watergirl.y > (33 + h) && watergirl.y < 56)
			watergirl.y = 56;
		else if(watergirl.x < 138 && watergirl.y > (1 + h) && watergirl.y < 34)
			watergirl.y = 26;
	}
	if(watergirl.jumped && !watergirl.jumped2){
		if(watergirl.x > 133 && (watergirl.y + jumparr[j]) < (60 + h) && watergirl.y < 110 && watergirl.y > 60){
			watergirl.y = 60 + h;
			watergirl.jumped = 0;
		}
		else if(watergirl.x < 23 && (watergirl.y + jumparr[j]) < (29 + h) && watergirl.y < 81 && watergirl.y > 29){
			watergirl.y = 29 + h;
			watergirl.jumped = 0;
		}
		else if(watergirl.x > 137 && (watergirl.y + jumparr[j]) < (2 + h) && watergirl.y > 56){
			watergirl.y = 2 + h;
			watergirl.jumped = 0;
		}
		else if(watergirl.x > 143 && (watergirl.y + jumparr[j]) > 111 && watergirl.y > 60){
			watergirl.y = 60 + h;
			watergirl.jumped = 0;
		}
		else if(((watergirl.x > (142 - w)) || watergirl.x < 23) && (watergirl.y + jumparr[j]) < 124){
			watergirl.y += jumparr[j];
			j = (j + 1) % 19;
			if(j == 0)
				watergirl.jumped = 0;
		}
		watergirl.moved = 1;
	}
	if(watergirl.jumped){
		if(watergirl.x > (144-w) && watergirl.y < 124 && watergirl.y > 107 && jumparr[j] > 0)
			watergirl.jumped = 0;
	}
	if(!watergirl.jumped){
		j = 0;
		if(watergirl.x > 137 && (watergirl.y < 56) && watergirl.y > 2)
			watergirl.y = 56;
		else if(watergirl.x < (23 - w) && watergirl.y < 81 && watergirl.y > 29)
			watergirl.y = 81;
		else if(watergirl.x > (144 - w) && watergirl.y > 60 && watergirl.y < 124)
			watergirl.y = 111;
	}
	watergirl.jumped2 = 0;
}


void jump(int data){
	if(!fireboy.jumped && (data & 0x01))
		fireboy.jumped = 1;
	else if(!watergirl.jumped && ((data & 0x02) >> 1))
		watergirl.jumped = 1;
}


void jumpsound(void){
	static uint8_t last1 = 0, last2 = 0, now1, now2;
	if(!fireboy.jumped)
			last1 = 0;
	if(!watergirl.jumped)
			last2 = 0;
	now1 = fireboy.jumped;
	now2 = watergirl.jumped;
	if(last1 != now1){
		Sound_fbjump();
		last1 = now1;
	}
	if(last2 != now2){
		Sound_wgjump();
		last2 = now2;
	}
}


void erase(void){
	if(fireboy.x > fireboy.xold && fireboy.y == fireboy.yold)
		ST7735_FillRect(fireboy.xold,fireboy.yold-h+1,dx,h,0x1186);
	else if(fireboy.x < fireboy.xold && fireboy.y == fireboy.yold)
		ST7735_FillRect(fireboy.xold+w-2,fireboy.yold-h+1,dx,h,0x1186);
	else if(fireboy.x > fireboy.xold && fireboy.y < fireboy.yold){ 
		ST7735_FillRect(fireboy.xold,fireboy.yold-h+1,fireboy.x-fireboy.xold+1,h,0x1186);
		ST7735_FillRect(fireboy.x,fireboy.y,fireboy.xold+w-fireboy.x,fireboy.yold-fireboy.y+1,0x1186);
	}
	else if(fireboy.x < fireboy.xold && fireboy.y < fireboy.yold){
		ST7735_FillRect(fireboy.xold,fireboy.y,fireboy.x+w-1-fireboy.xold+1,fireboy.yold-fireboy.y+1,0x1186);
		ST7735_FillRect(fireboy.x+w-1,fireboy.yold-h+1,fireboy.xold-fireboy.x+1,h,0x1186);
	}
	else if(fireboy.x == fireboy.xold && fireboy.y < fireboy.yold)
		ST7735_FillRect(fireboy.x,fireboy.y,w,fireboy.yold-fireboy.y+1,0x1186);
	else if(fireboy.x > fireboy.xold && fireboy.y > fireboy.yold){ 
		ST7735_FillRect(fireboy.xold,fireboy.yold-h+1,fireboy.x-fireboy.xold+1,h,0x1186);
		ST7735_FillRect(fireboy.x,fireboy.yold-h+1,fireboy.xold+w-fireboy.x,fireboy.y-fireboy.yold+1,0x1186);
	}
	else if(fireboy.x < fireboy.xold && fireboy.y > fireboy.yold){ 
		ST7735_FillRect(fireboy.xold,fireboy.yold-h+1,fireboy.x+w-fireboy.xold,fireboy.y-fireboy.yold+1,0x1186);
		ST7735_FillRect(fireboy.x+w-1,fireboy.yold-h+1,fireboy.xold-fireboy.x+1,h,0x1186);
	}
	else if(fireboy.x == fireboy.xold && fireboy.y > fireboy.yold)
		ST7735_FillRect(fireboy.x,fireboy.yold-h+1,w,fireboy.y-fireboy.yold+1,0x1186);
	if(watergirl.x > watergirl.xold && watergirl.y == watergirl.yold)
		ST7735_FillRect(watergirl.xold,watergirl.yold-h+1,dx,h,0x1186);
	else if(watergirl.x < watergirl.xold && watergirl.y == watergirl.yold)
		ST7735_FillRect(watergirl.xold+w-2,watergirl.yold-h+1,dx,h,0x1186);
	else if(watergirl.x > watergirl.xold && watergirl.y < watergirl.yold){ 
		ST7735_FillRect(watergirl.xold,watergirl.yold-h+1,watergirl.x-watergirl.xold+1,h,0x1186);
		ST7735_FillRect(watergirl.x,watergirl.y,watergirl.xold+w-watergirl.x,watergirl.yold-watergirl.y+1,0x1186);
	}
	else if(watergirl.x < watergirl.xold && watergirl.y < watergirl.yold){
		ST7735_FillRect(watergirl.xold,watergirl.y,watergirl.x+w-1-watergirl.xold+1,watergirl.yold-watergirl.y+1,0x1186);
		ST7735_FillRect(watergirl.x+w-1,watergirl.yold-h+1,watergirl.xold-watergirl.x+1,h,0x1186);
	}
	else if(watergirl.x == watergirl.xold && watergirl.y < watergirl.yold)
		ST7735_FillRect(watergirl.x,watergirl.y,w,watergirl.yold-watergirl.y+1,0x1186);
	else if(watergirl.x > watergirl.xold && watergirl.y > watergirl.yold){ 
		ST7735_FillRect(watergirl.xold,watergirl.yold-h+1,watergirl.x-watergirl.xold+1,h,0x1186);
		ST7735_FillRect(watergirl.x,watergirl.yold-h+1,watergirl.xold+w-watergirl.x,watergirl.y-watergirl.yold+1,0x1186);
	}
	else if(watergirl.x < watergirl.xold && watergirl.y > watergirl.yold){ 
		ST7735_FillRect(watergirl.xold,watergirl.yold-h+1,watergirl.x+w-watergirl.xold,watergirl.y-watergirl.yold+1,0x1186);
		ST7735_FillRect(watergirl.x+w-1,watergirl.yold-h+1,watergirl.xold-watergirl.x+1,h,0x1186);
	}
	else if(watergirl.x == watergirl.xold && watergirl.y > watergirl.yold)
		ST7735_FillRect(watergirl.x,watergirl.yold-h+1,w,watergirl.y-watergirl.yold+1,0x1186);
}


void draw(void){
	erase();
	checkgem();
	ST7735_DrawBitmap(4,26,enddoor,18,22);
	ST7735_DrawBitmap(107,2,topright,50,3);
	ST7735_DrawBitmap(fireboy.x,fireboy.y,boy,w,h);
	ST7735_DrawBitmap(watergirl.x,watergirl.y,girl,w,h);
	if(language == English)
		ST7735_SetCursor(15,0);
	else if(language == Spanish)
		ST7735_SetCursor(17,0);
	if(score == 1000)
		ST7735_OutUDec(999);
	else
		ST7735_OutUDec(score);
	fireboy.moved = 0;
	watergirl.moved = 0;
}


void start(void){
	ST7735_FillScreen(0x1186);
	ST7735_DrawBitmap(20,112,blankending,120,96);
	ST7735_DrawString(5,4,fireboystring,ST7735_RED);
	ST7735_DrawString(13,4,watergirlstring,ST7735_BLUE);
	ST7735_SetCursor(12,4);
	ST7735_OutChar('&');
	while(game == starting){
		static uint8_t select = 0, select2 = 0, screen = 0, lastscreen = 0;
		if(!screen){
			if(lastscreen != screen){
				ST7735_SetCursor(9,6);
				ST7735_OutString("         ");
				ST7735_SetCursor(9,7);
				ST7735_OutString("         ");
				lastscreen = screen;
			}
			if(language == English){
				ST7735_SetCursor(11,6);
				ST7735_OutString("Play");
				ST7735_SetCursor(9,7);
				ST7735_OutString("Languages");
				if(!select){	
					ST7735_SetCursor(8,7);
					ST7735_OutChar(' ');
					ST7735_SetCursor(18,7);
					ST7735_OutChar(' ');
					ST7735_SetCursor(8,6);
					ST7735_OutChar('<');
					ST7735_SetCursor(18,6);
					ST7735_OutChar('>');
				}
				else if(select){
					ST7735_SetCursor(8,6);
					ST7735_OutChar(' ');
					ST7735_SetCursor(18,6);
					ST7735_OutChar(' ');				
					ST7735_SetCursor(8,7);
					ST7735_OutChar('<');
					ST7735_SetCursor(18,7);
					ST7735_OutChar('>');
				}		
			}
			else if(language == Spanish){
				ST7735_SetCursor(11,6);
				ST7735_OutString("Jugar");
				ST7735_SetCursor(10,7);
				ST7735_OutString("Idiomas");
				if(!select){	
					ST7735_SetCursor(8,7);
					ST7735_OutChar(' ');
					ST7735_SetCursor(18,7);
					ST7735_OutChar(' ');					
					ST7735_SetCursor(8,6);
					ST7735_OutChar('<');
					ST7735_SetCursor(18,6);
					ST7735_OutChar('>');
				}
				else if(select){
					ST7735_SetCursor(8,6);
					ST7735_OutChar(' ');
					ST7735_SetCursor(18,6);
					ST7735_OutChar(' ');				
					ST7735_SetCursor(8,7);
					ST7735_OutChar('<');
					ST7735_SetCursor(18,7);
					ST7735_OutChar('>');
				}
			}
			while(!watergirl.jumped && !fireboy.jumped){};
			if(watergirl.jumped){
				select = select ^ 1;
				watergirl.jumped = 0;
				Sound_wgjump();
			}
			else if(fireboy.jumped && !select){
				game = playing;
				fireboy.jumped = 0;
				Sound_winner();
			}
			else if(fireboy.jumped && select){
				fireboy.jumped = 0;
				select = 0;
				screen = 1;
				Sound_fbjump();
			}
		}
		if(screen){
			if(lastscreen != screen){
				ST7735_SetCursor(9,6);
				ST7735_OutString("         ");
				ST7735_SetCursor(9,7);
				ST7735_OutString("         ");
				lastscreen = screen;
			}
			if(language == English){
				ST7735_SetCursor(10,6);
				ST7735_OutString("English");
				ST7735_SetCursor(10,7);
				ST7735_OutString("Spanish");
				if(!select2){	
					ST7735_SetCursor(8,7);
					ST7735_OutChar(' ');
					ST7735_SetCursor(18,7);
					ST7735_OutChar(' ');						
					ST7735_SetCursor(8,6);
					ST7735_OutChar('<');
					ST7735_SetCursor(18,6);
					ST7735_OutChar('>');
				}
				else if(select2){
					ST7735_SetCursor(8,6);
					ST7735_OutChar(' ');
					ST7735_SetCursor(18,6);
					ST7735_OutChar(' ');					
					ST7735_SetCursor(8,7);
					ST7735_OutChar('<');
					ST7735_SetCursor(18,7);
					ST7735_OutChar('>');
				}
			}
			else if(language == Spanish){
				ST7735_SetCursor(10,6);
				ST7735_OutString("Ingl\x82s");
				ST7735_SetCursor(10,7);
				ST7735_OutString("Espa\xA4ol");
				if(!select2){	
					ST7735_SetCursor(8,7);
					ST7735_OutChar(' ');
					ST7735_SetCursor(18,7);
					ST7735_OutChar(' ');					
					ST7735_SetCursor(8,6);
					ST7735_OutChar('<');
					ST7735_SetCursor(18,6);
					ST7735_OutChar('>');
				}
				else if(select2){
					ST7735_SetCursor(8,6);
					ST7735_OutChar(' ');
					ST7735_SetCursor(18,6);
					ST7735_OutChar(' ');						
					ST7735_SetCursor(8,7);
					ST7735_OutChar('<');
					ST7735_SetCursor(18,7);
					ST7735_OutChar('>');
				}
			}
			while(!watergirl.jumped && !fireboy.jumped){};
			if(watergirl.jumped){
				select2 = select2 ^ 1;
				watergirl.jumped = 0;
				Sound_wgjump();
			}
			else if(fireboy.jumped && !select2){
				language = English;
				screen = 0;
				fireboy.jumped = 0;
				Sound_fbjump();
			}
			else if(fireboy.jumped && select2){
				language = Spanish;
				fireboy.jumped = 0;
				select2 = 0;
				screen = 0;
				Sound_fbjump();
			}
		}
	}
}



void play(void){
	Game_Init();
	while(game == playing){
		jumpsound();
		if(fireboy.moved || watergirl.moved){
			puddles();
			gems();
			draw();
		}
		if(status == dead){
			Sound_gameover();
			game = ending;
		}
		else if(fireboy.x < 8 && fireboy.y == 26 && watergirl.x < 8 && watergirl.y == 26){
			Sound_winner();
			game = ending;
		}
		checkplatform();
		moveplatform();
	}
}


void end(void){
	if(language == English)
		ST7735_DrawBitmap(20,112,deadscreen,120,96);
	else if(language == Spanish){
		ST7735_DrawBitmap(20,112,blankending,120,96);
		ST7735_SetCursor(6,5);
		ST7735_OutString("Juego Terminado");
		ST7735_SetCursor(7,7);
		ST7735_OutString("<Ir al Men\xA3>");
	}
	while(game == ending){
		if(fireboy.jumped || watergirl.jumped){
			fireboy.jumped = 0;
			watergirl.jumped = 0;
			game = starting;
		}
	}
}


int main(void){
  DisableInterrupts();
  PLL_Init(Bus80MHz);
  Sound_Init();
  ADC_Init();
  Output_Init();
	//ST7735_InvertDisplay(1);
  Timer0_Init(&move,80000000/10,5);
  Jump_Init(&jump,6);
	EnableInterrupts();
	while(1){
		start();
		play();
		end();
	}
}
