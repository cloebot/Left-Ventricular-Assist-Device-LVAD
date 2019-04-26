#ifndef PULSE_SENSOR_H
#define PULSE_SENSOR_H

#include <stdint.h>
#include <stdio.h>
#include "tm4c123gh6pm.h"


void ADC_Init();
void pulseSensor_Init();
extern volatile int BPM;
#endif 