#include "timer.h"

//********************************Handler**************************************

// timer handler is on every one second
void Timer_Handler( void ) {
  // GPIOF_DATA_BLUE = ~GPIOF_DATA_BLUE; //TESTING
  ADC0_PSSI_R |= 0x8; // Begin sampling on Sample Sequencer 3
  TIMER0_ICR_R |= (1<<0);  // reset timer
}


//*********************************Initialization*******************************

// initialize & setup Timer registers
void Timer0_Init() {
  SYSCTL_RCGCTIMER_R |= 0x01; // enable Timer 0
  
  TIMER0_CTL_R &= 0xFE; // deactivate timer
  TIMER0_CFG_R = 0x0; // set timer configuration - select 32 bit timer
  TIMER0_TAMR_R = ((TIMER0_TAMR_R & 0xFEE) | 0x002); // change mode(periodic) and direction(count down) of the counter
  TIMER0_ICR_R = 0x1; // clear the flag
  
  TIMER0_CTL_R |= 0x01; // activate the timer
  TIMER0_IMR_R |= 0x01; // enable time-out interrupt mask
  
  TIMER0_TAILR_R = 0x00007D00;//counter setup for 4MHZ(default)
  NVIC_EN0_R |= (1<<19); // enable interrupt 19 for timer 
}