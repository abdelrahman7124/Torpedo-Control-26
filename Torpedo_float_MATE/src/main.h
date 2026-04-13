/*
Author: Marwan Aly
Project: Float System
Team: Torpedo ROV
Discrption: The main file that handles the whole system
*/


#ifndef MAIN_H
    #define MAIN_H
    #include <Arduino.h>

    #include "wireless.h"
    #include "filters.h"
    #include "imu.h"
    #include "pid.h"
    #include "bmp.h"

    wireless connection;
    BMP pressure;
    String msg;
    float depth;
    float pressure_value;
    String time_stamp;
      
#endif 