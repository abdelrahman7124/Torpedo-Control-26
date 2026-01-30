#ifndef CONFIG_H
#define CONFIG_H
#include <Arduino.h>
#include <SPI.h>
#include "Wire.h"




/*-----------------------Pressure sensor----------------------------*/
#define MCLK 14
#define MOSI 13
#define MISO 34
#define SCLK 12
#define pwm_Channel 14 // PWM channel (0-15)
#define pwm_Resolution 8  // PWM resolution (bits)
#define pwm_Freq 30000    // Desired PWM frequency in Hz
#define MOTORS_COUNT 6
#define TOOLS_COUNT 8

/*---------------------------------------------------*/

#define ACTUAL_DATA 13        // (1 Accessories | 6 * 2 Motor )(bytes)

//Nanoz data Indices
#define MOTORS_SPEEDS_INDEX 1
// #define DIRECTIONS_BYTE_INDEX 1
#define ACC_BYTE_INDEX 0

//Motors speeds
#define MOT_ZERO_SPEED 1500
#define MOT_MAX_SPEED 1900
#define MOT_MIN_SPEED 1100


//Communication
#define BOX_PORT 8000
#define CONSOLE_PORT 7000

/*--------UDP--------*/
#define UDP_REC_FRAME 13  // 1 Accessories,  6 thrusters speeds * 2 bytes
#define UDP_SEND_FRAME 10 // for communication test

/*-------MUX-------*/
#define LatchPin 0
#define ClockPin 1
#define DataPin 2
#define SHIFTREGISTER 3


//debuging
#define THRUSTERS_PRINT_ON
#define ACC_PRINT_ON







#endif
