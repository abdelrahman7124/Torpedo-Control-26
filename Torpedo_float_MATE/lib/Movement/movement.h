/*
Author: Marwan Aly
Project: Movement Logic
Team: Torpedo ROV
Description: Handles the movement logic of the system
*/

#ifndef MOVEMENT_H
    #define MOVEMENT_H
    #include "movement_config.h"
    #include "control.h"
    #include "bmp.h"
    #include <pid.h>

    enum MovementState 
    { 
        IDLE = 0, 
        DIVING = 1, 
        HOVERING = 2, 
        SURFACING = 3,
        MISSION_COMPLETE = 4,
        FIRST_DIVE_COMPLETE = 5,
        SECOND_DIVE_COMPLETE = 6,
        FIRST_STOP_COMPLETE = 7,
        SECOND_STOP_COMPLETE = 8,
        RESET = 9,
    };

    class Movement
    {
        private:
            BMP bmp;
            Control control;
            PID pid;

            int power_up;
            int power_down;
            
            float current_depth;
            float target_depth;
            float prev_target_depth;
            
            unsigned long movement_current_time;
            unsigned long movement_prev_time;
            double movement_dt;
            
            bool hold_flag;
            bool dive_flag;
            bool rise_flag;

        public:
            Movement();
            MovementState dive(float depth);
            MovementState hold(float time);
            MovementState rise_to_surface();
    };


#endif