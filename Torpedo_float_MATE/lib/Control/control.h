/*
Author: Marwan Aly
Project: Controls
Team: Torpedo ROV
Discrption: Handles the basic control logic of the system 
*/

#ifndef CONTROL_H
    #define CONTROL_H
    #include <Arduino.h>
    #include "config.h"
    #include "pid.h"
    #include "bmp.h"

    class Control 
    {
        private:
            PID pid;
            BMP bmp;
            volatile float goal;
            volatile float reading;
            bool balance_flag;
            double time;
            double prev_time;
            double dt;
            int power_up;
            int power_down;


        public:
            Control();
            void up();
            void down();
            void stop();
            void setPower_up(int power);
            void setPower_down(int power);
            void hover();
    };
    

#endif