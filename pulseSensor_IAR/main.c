// EE 474
// Lab 4
// Kyunghyun Cloe Lee
// Changing clock speeds using switches and show the internal temperture
// by displaying different LED colors

#include <stdint.h>
#include <stdio.h>
#include "registerAddr.h"
#include "timer.h"
#include "pulseSensor.h"

int main()
{
  pulseSensor_Init();
  Timer0_Init();
  ADC_Init();
  
  while (1) {}
  return 0;
}
