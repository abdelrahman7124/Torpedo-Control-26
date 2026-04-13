/*
Author: Marwan Aly
Project: BMP180 Pressure Sensor
Team: Torpedo ROV
Discrption: Handles interfaceing and reterival of data from the BMP180 pressure sensor
*/

#ifndef BMP_H
#define BMP_H
    
    #include <Adafruit_BMP085.h>
    #include <Wire.h>
    #include <Arduino.h>
    #include "config.h"

    class BMP
    {
        private:
            Adafruit_BMP085 bmp;
            float pressure;
            float depth;
            int time_sec;
            int time_min;
            int time_hr;
            unsigned long time;
            String time_sec_msg;
            String time_min_msg;
            String time_hr_msg;
            String time_stamp;

        
        public:
            BMP();
            void init();
            float readPressure();
            float readDepth();
            String getTime();
    };

#endif