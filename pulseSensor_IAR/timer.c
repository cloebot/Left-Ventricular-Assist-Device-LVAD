#include "timer.h"

//********************************Handler**************************************

// timer handler is on every one second
void Timer_Handler( void ) {
  // GPIOF_DATA_BLUE = ~GPIOF_DATA_BLUE; //TESTING
  ADC0_PSSI_R |= 0x8; // Begin sampling on Sample Sequencer 3
  GPTMICR |= (1<<0);  // reset timer
}


//*********************************Initialization*******************************

// initialize & setup Timer registers
void Timer0_Init() {
  RCGCTIMER |= 0x01; // enable Timer 0
  
  GPTMCTL &= 0xFE; // deactivate timer
  GPTMCFG = 0x0; // set timer configuration - select 32 bit timer
  GPTMTAMR = ((GPTMTAMR & 0xFEE) | 0x002); // change mode(periodic) and direction(count down) of the counter
  GPTMICR = 0x1; // clear the flag
  
  GPTMCTL |= 0x01; // activate the timer
  GPTMIMR |= 0x01; // enable time-out interrupt mask
  
  GPTMTAILR = 0x0000D700;//counter setup for 4MHZ(default)
  NVIC_EN0 |= (1<<19); // enable interrupt 19 for timer 
}