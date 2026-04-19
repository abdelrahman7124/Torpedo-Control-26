/*
Author: Marwan Aly
Project: Controls
Team: Torpedo ROV
Description: Handles the basic control logic of the system 
*/

#ifndef CONTROL_H
    #define CONTROL_H
    #include <Arduino.h>
    #include "control_config.h"
    #include "pid.h"
    #include "bmp.h"

    enum ControlState
    {
        MOVING_UP = 1,
        MOVING_DOWN = 2,
        STOPPING = 3,
    };


    class Control 
    {
        private:
        
        volatile float goal;
        volatile float reading;
        
        bool balance_flag;
        
        unsigned long control_current_time;
        unsigned long control_prev_time;
        double control_dt;
        
        int power_up;
        int power_down;
        
        
        public:
            BMP bmp;
            PID pid;
            Control();
            ControlState up();
            ControlState down();
            ControlState stop();
            void setPower_up(int power);
            void setPower_down(int power);
            ControlState hover();
    };
    

#endif