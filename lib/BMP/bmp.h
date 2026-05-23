/*
Author: Marwan Aly
Project: BMP180 Pressure Sensor
Team: Torpedo ROV
Description: Handles interfaceing and reterival of data from the BMP180 pressure sensor
*/

#ifndef BMP_H
#define BMP_H
    
    #include <Adafruit_BMP085.h>
    #include <Wire.h>
    #include <Arduino.h>
    #include "filters.h"
    #include "bmp_config.h"

    class BMP
    {
        private:
            Adafruit_BMP085 bmp;
            KALMAN  filter;
            float pressure;
            float depth;
            TwoWire I2C_BMP = TwoWire(1);

        public:
            BMP();
            void init();
            float readPressure();
            float readDepth();
    };

#endif