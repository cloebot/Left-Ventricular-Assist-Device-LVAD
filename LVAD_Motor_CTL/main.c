#include <stdio.h>
#include <stdint.h>
#include "motor_controller.h"
#include "pulseSensor.h"
#include "timer.h"

int main()
{ 
  pulseSensor_Init();
  Timer0_Init();
  ADC_Init();
  MotorConfigure();
  

  while (1) {
    Rotate();
  }
  return 0;
}
