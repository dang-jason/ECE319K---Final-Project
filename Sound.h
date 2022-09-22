// Sound.h
// Runs on TM4C123 or LM4F120
// Prototypes for basic functions to play sounds from Fireboy and Watergirl
// Gabriel Mi & Jason Dang
// 5/3/2022

#ifndef SOUND_H
#define SOUND_H
#include <stdint.h>

void Sound_Init(void);
void Sound_Start(const uint8_t *pt, uint32_t count);
void Sound_dying(void);
void Sound_gem(void);
void Sound_gameover(void);
void Sound_fbjump(void);
void Sound_wgjump(void);
void Sound_platmove(void);
void Sound_winner(void);

#endif
