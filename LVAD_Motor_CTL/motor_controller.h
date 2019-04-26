#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#define PART_TM4C123GH6PM

#include "tm4c123gh6pm.h"
#include "pulseSensor.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"

  // Configure registers relevant to motors.
  void MotorConfigure();

  // Move one step forward.
  void Rotate();

  // Configure PWM to be able to change motor speed.    
  void ConfigurePWM();

  // Configure GPIO Port E to be able to change motor direction.
  void ConfigureGPIO();

  // Configure timer for time based moving actions.
  void ConfigureTimer();

  // Set motor speed in range (0,400).
  void SpeedAdjustment(int level);

  // Set timer value and enable timer.
  void StartTimer(int load_value);

  // Disable timer.
  void EndTimer();


#endif //MOTOR_CONTROLLER_H
