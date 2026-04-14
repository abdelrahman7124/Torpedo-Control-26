/*
Author: Marwan Aly
Project: Float System
Team: Torpedo ROV
Description: The main file that handles the whole system
*/


#ifndef MAIN_H
    #define MAIN_H
    #include <Arduino.h>
    #include <freertos/FreeRTOS.h>
    #include "wireless.h"
    #include "filters.h"
    #include "pid.h"
    #include "bmp.h"
    #include "system_check.h"

    wireless connection;
    BMP pressure;
    String msg;
    float depth;
    float pressure_value;
    String time_stamp;
      
#endif 