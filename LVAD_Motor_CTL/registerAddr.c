/* 
Kyunghyn Lee
EE 474

Header file for register address.*/
#ifndef REG_H
#define REG_H

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

#endif //REG_H