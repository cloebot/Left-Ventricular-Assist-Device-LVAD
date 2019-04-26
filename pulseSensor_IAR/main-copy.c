// EE 474
// Lab 3
// Kyunghyun Cloe Lee
// Blinking all LED lights every one second

#include <stdint.h>

// gpio registers
#define GPIO_RCGCGPIO (*((volatile unsigned long *)0x400FE608))
#define GPIO_GPIODIR (*((unsigned long *)0x40025400))
#define GPIO_GPIODEN (*((unsigned long *)0x4002551C))
#define GPIOF_DATA (*((volatile unsigned long *)0x400253FC))
#define GPIO_LOCK (*((unsigned long *)0x40025520))
#define GPIO_CR (*((unsigned long *)0x40025524))
#define GPIO_PULLUP (*((unsigned long *)0x40025510))
#define GPIOF_DATA_BLUE (*((volatile unsigned long *)0x40025010))

// timer registers
#define RCGCTIMER (*((volatile unsigned long *)0x400FE604))
#define GPTMCTL  (*((volatile unsigned long *)0x4003000C))
#define GPTMCFG  (*((volatile unsigned long *)0x40030000))
#define GPTMTAMR (*((volatile unsigned long *)0x40030004))
#define GPTMTAILR (*((volatile unsigned long *)0x40030028))
#define GPTMRIS  (*((volatile unsigned long *)0x4003001C))
#define GPTMICR  (*((volatile unsigned long *)0x40030024))
#define GPTMIMR (*((volatile unsigned long *)0x40030018))

//interrupt registers
#define GPIOF_IM (*((volatile unsigned long *)0x40025410))
#define GPIOF_RIS (*((volatile unsigned long *)0x40025414))
#define GPIOF_ICR (*((volatile unsigned long *)0x4002541C))
#define GPIOF_MIS (*((volatile unsigned long *)0x40025418))

#define NVIC_EN0 (*((volatile unsigned long *)0xE000E100))
#define GPIO_IS  (*((volatile unsigned int *)0x40025404)) 
#define GPIO_IBE (*((volatile unsigned int *)0x40025408)) 
#define GPIO_IEV (*((volatile unsigned int *)0x4002540C))

//PLL registers
#define SYSCTL_RCC_R (*((volatile unsigned int *)0x400FE060)) //run mode clock configuration
#define SYSCTL_RCC2_R (*((volatile unsigned int *)0x400FE070)) 

// ADC registers
#define SYSCTL_RCGCADC_R (*((volatile unsigned int *)0x400FE638)) // Analog-to-Digital Converter Run Mode
#define ADC_ACTSS_R      (*((volatile unsigned int *)0x40038000)) // ADC Active Sample Sequencer
#define ADC_EMUX_R       (*((volatile unsigned int *)0x40038014)) //ADC Event Multiplexer Select
#define ADC_SSCTL3_R     (*((volatile unsigned int *)0x400380A4)) //ADC Sample Sequence Control 3
#define ADC_IM_R         (*((volatile unsigned int *)0x40038008)) // ADC Interrupt Mask
#define ADC0_PSSI_R      (*((volatile unsigned int *)0x40038028)) // ADC Processor Sample Sequence Initiate
#define ADC0_RIS_R       (*((volatile unsigned int *)0x40038004)) //  ADC Raw Interrupt Status
#define ADC0_SSFIFO3_R   (*((volatile unsigned int *)0x400380A8)) // ADC Sample Sequence Result
#define ADC0_ISC_R       (*((volatile unsigned int *)0x4003800C)) // Interrupt Status and Clear

// gpioE port register
#define GPIOE_AFSEL_R (*((volatile unsigned int *)0x40038420)) // Alternate Function Select
#define GPIOE_DEN_R (*((volatile unsigned int *)0x4002451C))
#define GPIOE_AMSEL_R (*((volatile unsigned int *)0x40024528)) // Analog Mode Select

void ADC_Init();
void Timer0_Init();
void PLL_Init ();
void PortF_Init ();
void Interrupt_Init();
void Config_80MHZ();
void Config_4MHZ();

int main()
{
  Timer0_Init();
  PLL_Init();
  ADC_Init();
  PortF_Init();
  Interrupt_Init();
  
  while (1) {}
  return 0;
}

//********************************Handler**************************************

// interrupt for handler
void Gpio_PortF_Handler( void ) {
  if((GPIOF_DATA & 0x11) == 0x01) { 
    Config_4MHZ(); 
  } else if ((GPIOF_DATA & 0x11) == 0x10) {
    Config_80MHZ();
  }
  GPIOF_ICR |= 0x11; //clear port0 interrupt
}

void Timer_Handler( void ) {
  // GPIOF_DATA_BLUE = ~GPIOF_DATA_BLUE; //TESTING
  ADC0_PSSI_R |= 0x8; // Begin sampling on Sample Sequencer 3
  GPTMICR |= (1<<0); //reset timer
}

void ADC0_Handler( void ) {
  // GPIOF_DATA_BLUE = ~GPIOF_DATA_BLUE; //TESTING
  float temp = (147.5 - (247.5 * (ADC0_SSFIFO3_R)) / 4096.0);
    if(0<= temp  && temp < 17 ) {
    GPIOF_DATA = 0x02; //red
  } else if (17<= temp  && temp < 19) {
    GPIOF_DATA = 0x04; //blue
  } else if (19 <= temp && temp < 21) {
    GPIOF_DATA = 0x06; //violet
  } else if (21 <= temp && temp < 23) {
    GPIOF_DATA = 0x08; //green
  } else if (23 <= temp && temp < 25) {
    GPIOF_DATA = 0x0A; //yellow
  } else if (25 <= temp && temp < 27) {
    GPIOF_DATA = 0x0C; //lightblue
  } else if (27 <= temp && temp < 40) {
    GPIOF_DATA = 0x0E; //white
  }
  ADC0_ISC_R |= (1<<3); // clear completion flag
}

//*********************************Initialization*******************************

// initize & setup Timer registers
void Timer0_Init() {
  RCGCTIMER |= 0x01; // enable Timer 0
  
  GPTMCTL &= 0xFE; // deactivate timer
  GPTMCFG = 0x0; // set timer configuration - select 32 bit timer
  GPTMTAMR = ((GPTMTAMR & 0xFEE) | 0x002); // change mode(periodic) and direction(count down) of the counter
  GPTMICR = 0x1; // clear the flag
  
  GPTMCTL |= 0x01; // activate the timer
  
  GPTMTAILR = 0x003D0900;//counter setup for 4MHZ(default)
  GPTMIMR |= 0x01; // enable time-out interrupt mask
}

// initize & setup Interrupt registers
void Interrupt_Init() {
  // enable interrupt 19 for timer 0A and 30 for gpio portF
  NVIC_EN0 |= 0x40080000;
  NVIC_EN0 |= (1<<17);// enable interrupt 17 for ADC
}

// initize & setup GPIO registers
void PortF_Init () {
  GPIO_RCGCGPIO = 0x20; // enable port F GPIO (RCGCGPIO)
  GPIO_GPIODIR = 0x0E; // set port F, red,blue,green led as Output and  switches as Input (GPIODIR)
  GPIO_GPIODEN = 0x1F; //enable digital PortF (GPIODEN)
  GPIO_LOCK = 0x4C4F434B; //unlocks the GPIO commit
  GPIO_CR = 0x1F;   // make FP0-4 writable
  GPIO_PULLUP = 0x11;   // pull up setting sw1,2
  //interrupt
   GPIO_IS &= ~0x11; // bit 4,0 edge sensitive 
  GPIO_IBE &= ~0X11; // trigger is controlled by IEV
  GPIO_IEV = ~0X11; // falling edge trigger 
  GPIOF_IM |= 0x11; //enable interrupt for PF0 and PF4 pin
}

// initize the Phase Lock Loop module
void PLL_Init () {
    SYSCTL_RCC2_R |= 0x80000000; //use Rcc2 register (allow to override the RCC field)
    SYSCTL_RCC2_R |= 0x00000800; // bypass = 1 (clock running during configuration)
    //setup for 4MHZ(default)
    //SYSCTL_RCC_R &= ~0x00000030; //OSCSRC = 00 (4-5bit)
    SYSCTL_RCC2_R &= ~0x00000070; //OSCSRC = 00 (4-6bit)
    
    //SYSCTL_RCC_R &= ~0x00002000; // NOT SURE: Activate PLL by clearing PWRDN
    SYSCTL_RCC2_R &= ~0x00002000; // Activate PLL by clearing PWRDN2
    
    SYSCTL_RCC_R |= (1<<22); // USESYSDIV set = 1
    SYSCTL_RCC2_R = (SYSCTL_RCC2_R & ~0x1FC00000) // clear SYSDIV(28-22bits)
                 + (3<<22); // 16MHz/(SYSDIV+1)
   
}

void ADC_Init() {
  GPIO_RCGCGPIO |= 0x10; // enable portE(AIN0 is on PE3)
  
  SYSCTL_RCGCADC_R |= 0x1; // enable ADC module 0 (provide the clock)
  while ((SYSCTL_RCGCADC_R & 0x0000001) == 0) {};
  
  GPIOE_AFSEL_R |= 0x8; // enable alternate function (TEMP)
  GPIOE_DEN_R &= ~0x8; // disable digital function
  GPIOE_AMSEL_R |= 0x8; // enable analog function
  
  ADC_ACTSS_R &= ~0x8; // disable SS3 during configuration
  ADC_EMUX_R |= ~0xF000;//0x5000; // software trigger conversion (Timer)
  
  // set ADC CTL
  ADC_SSCTL3_R |= 0x6; // take one sample at a time, set flag at 1st sample
  ADC_SSCTL3_R |= (1<<3); // Read from Internal Temperature Sensor
  
  ADC_ACTSS_R |= 0x8; // enable ADC0 sequencer 3
  ADC_IM_R |= (1<<3); //Masking interrupt for SS3
  
  ADC0_ISC_R |= (1<<3); // clear completion flag once before start
}

//******************************************************************************
void Config_80MHZ() {
    
     //setup for 80MHZ
    SYSCTL_RCC2_R |= 0x80000000; //use Rcc2 register (allow to override the RCC field)
    SYSCTL_RCC2_R |= 0x00000800; // bypass = 1 (clock running during configuration)
    
    SYSCTL_RCC_R = (SYSCTL_RCC_R & ~0x000007C0) // clear Xtal field first
                 + 0x00000540; // 10101 select 16MHz crystal
    
    SYSCTL_RCC2_R &= ~0x00000070; //OSCSRC = 00 (4-6bit)
    SYSCTL_RCC2_R &= ~0x00002000; // Activate PLL by clearing PWRDN2
    SYSCTL_RCC2_R |= 0x4000000; //DIV400(bit30) = 1 -> select 400MHz (0=200mhz)
    
     // count 80M 
    SYSCTL_RCC2_R = (SYSCTL_RCC2_R & ~0x1FC00000) // clear SYSDIV(28-22bits)
                 + (4<<22); // 400MHz/(SYSDIV+1)   
    while ((SYSCTL_RCC_R & 0x00000040) == 0) {}; //wait until PLLRIS(6bit) = 1 (WHERE TO GO(?))
    
    SYSCTL_RCC2_R &= ~0x00008000; // BYPASS = 0 and use PLL
    
    GPTMTAILR = 0x04C4B400; //count 80M
}

void Config_4MHZ() {
    
    SYSCTL_RCC2_R = (SYSCTL_RCC2_R & ~0x1FC00000) // clear SYSDIV(28-22bits)
                 + (3<<22); // 16MHz/(SYSDIV+1)
    SYSCTL_RCC2_R |= 0x00000800; // bypass = 1 (clock running during configuration)
    
    GPTMTAILR = 0x003D0900; // count 4M 
}
