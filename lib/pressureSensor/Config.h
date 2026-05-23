#ifndef CONFIG_H
#define CONFIG_H
#include <Arduino.h>
#include <SPI.h>
#include "Wire.h"




/*-----------------------Pressure sensor----------------------------*/
#define MCLK 14
#define MOSI 13
#define MISO 34
#define SCLK 15
#define pwm_Channel 14 // PWM channel (0-15)
#define pwm_Resolution 8  // PWM resolution (bits)
#define pwm_Freq 30000    // Desired PWM frequency in Hz
#define MOTORS_COUNT 6
#define TOOLS_COUNT 8

/*---------------------------------------------------*/

#endif