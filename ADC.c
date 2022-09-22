// ADC.c
// Runs on TM4C123
// Provide functions that initialize ADC0
// Last Modified: 5/3/2022
// Student names: Jason Dang and Gabriel Mi
// Last modification date: 5/3/2022

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

// ADC initialization function 
// Input: none
// Output: none
// measures from PE4 and PE5, analog channel 8/9
void ADC_Init(void){ 
  SYSCTL_RCGCADC_R |= 0x00000001; // 1) activate ADC0
  SYSCTL_RCGCGPIO_R |= 0x10; // 1) activate clock for Port E
	while((SYSCTL_PRGPIO_R&0x10) == 0){};
	__nop();
	__nop();
  GPIO_PORTE_DIR_R &= ~0x30;      // 3) make PE4 PE5 input
  GPIO_PORTE_AFSEL_R |= 0x30;     // 4) enable alternate function on PE4 PE5
  GPIO_PORTE_DEN_R &= ~0x30;      // 5) disable digital I/O on PE4 PE5
  GPIO_PORTE_AMSEL_R |= 0x30;     // 6) enable analog functionality on PE4 PE5
  ADC0_PC_R &= ~0xF;              // 8) clear max sample rate field
  ADC0_PC_R |= 0x1;               //    configure for 125K samples/sec
  ADC0_SSPRI_R = 0x3210;          // 9) Sequencer 3 is lowest priority
  ADC0_ACTSS_R &= ~0x0004;        // 10) disable sample sequencer 2
  ADC0_EMUX_R &= ~0x0F00;         // 11) seq2 is software trigger
  ADC0_SSMUX2_R = (ADC0_SSMUX2_R&0xFFFFFF00)+0x89;         // 12) set channels for SS2
  ADC0_SSCTL2_R = 0x0060;         // 13) no TS0 D0 IE0 END0 TS1 D1, yes IE1 END1
  ADC0_IM_R &= ~0x0004;           // 14) disable SS2 interrupts
  ADC0_ACTSS_R |= 0x0004;         // 15) enable sample sequencer 2

}

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
// measures from PE4, analog channel 9
// data[0] = fireboy PE4 AIN9
// data[1] = watergirl PE5 AIN8
void ADC_In(uint16_t data[2]){ 
  ADC0_PSSI_R = 0x0004;            // 1) initiate SS2
  while((ADC0_RIS_R&0x04)==0){};   // 2) wait for conversion done
  data[0] = ADC0_SSFIFO2_R&0xFFF;  // 3A) read first result
  data[1] = ADC0_SSFIFO2_R&0xFFF;  // 3B) read second result
  ADC0_ISC_R = 0x0004;             // 4) acknowledge completion
}



