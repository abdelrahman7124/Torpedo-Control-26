/*
Author: Marwan Aly
Project: Movement Logic
Team: Torpedo ROV
Description: Handles the movement logic of the system
*/

#ifndef MOVEMENT_H
    #define MOVEMENT_H
    #include "config.h"
    #include "control.h"
    #include "bmp.h"
    #include <pid.h>

    class Movement
    {
        private:
            enum MovementState 
            { 
                IDLE = 0, 
                DIVING = 1, 
                HOLDING = 2, 
                SURFACING = 3,
            };

            BMP bmp;
            Control control;
            PID pid;
            int power_up;
            int power_down;
            float current_depth;
            float target_depth;
            float prev_target_depth;
            double time;
            double prev_time;
            double dt;
            bool hold_flag;
            bool dive_flag;
            bool rise_flag;

        public:
            Movement();
            int dive(float depth);
            int hold(float time);
            int rise_to_surface();
    };


#endif