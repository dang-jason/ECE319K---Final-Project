// Move.c
// Gabriel Mi & Jason Dang
// Last Modified: 5/3/2022
// Edge-triggered interrupt for jumping

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

void (*JumpTask)(int data); // user function

void Jump_Init(void(*task)(int data), uint32_t priority){
  SYSCTL_RCGCGPIO_R |= 0x00000010; // activate clock for port E
  while((SYSCTL_PRGPIO_R & 0x00000010) == 0){};
	JumpTask = task;        			   // user function
  GPIO_PORTE_DIR_R  &= ~0x03;      // make PE0 PE1 input
  GPIO_PORTE_DEN_R  |=  0x03;      // enable digital I/O on PE0 PE1
  GPIO_PORTE_IS_R   &= ~0x03;      // PE0 PE1 is edge-sensitive
  GPIO_PORTE_IBE_R  &= ~0x03;      // PE0 PE1 is not both edges
	GPIO_PORTE_IEV_R  |=  0x03;			 // PE0 PE1 rising edge event
  GPIO_PORTE_ICR_R   =  0x03;      // clear flag0
  GPIO_PORTE_IM_R   |=  0x03;      // arm interrupt on PE0 PE1
  NVIC_PRI1_R = (NVIC_PRI1_R & 0xFFFFFF0F) | (priority << 5);  // Set Priority Level
  NVIC_EN0_R = 0x00000010;         // enable interrupt 4 in NVIC  
}

void GPIOPortE_Handler(void){
		JumpTask(GPIO_PORTE_RIS_R);
		GPIO_PORTE_ICR_R = 0x03;     // acknowledge PE0 PE1
}
