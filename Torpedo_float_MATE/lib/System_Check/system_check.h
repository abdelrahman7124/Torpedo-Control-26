/*
Author: Marwan Aly
Project: System Check for debugging
Team: Torpedo ROV
Description: Checks all sencors,acutators and communications of the system to ensure they are working properly. This is used for debugging and testing purposes only, and should not be used in the final version of the system.
*/

#ifndef SYSTEM_CHECK_H
    #define SYSTEM_CHECK_H
    #include "system_check_config.h"
    #include <Arduino.h>
    #include "control.h"
    #include "bmp.h"
    #include "rtc.h"
    #include "wireless.h"
    
    class SystemCheck
    {
    private:
        BMP bmp;
        Control control;
        wireless connection;
        RTC rtc;

        ControlState motor_state;
        bool direction_flag;

        float pressure;
        float depth;
        String time_stamp;

        void checkSensors();
        void checkActuators();
        void checkCommunication();
    
    public:
        SystemCheck();
        void checkSystem();
    };

#endif