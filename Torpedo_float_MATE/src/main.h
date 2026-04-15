/*
Author: Marwan Aly
Project: Float System
Team: Torpedo ROV
Description: The main file that handles the whole system
*/


#ifndef MAIN_H
    #define MAIN_H
    #include <Arduino.h>
    #include <queue>
    #include "wireless.h"
    #include "movement.h"
    #include "bmp.h"
    #include "system_check.h"
    #include "main_config.h"
    
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

    struct pressure_data
    {
        float depth;
        float pressure;
        String time_stamp;
    };

    void get_bmp_readings();
    void send_msg();
    int execute_mission();
    void sample_readings();
    String getTime();

    std::queue <pressure_data> pressure_log;

    wireless connection;
    BMP bmp;
    Movement action;

    float depth;
    float pressure;
    
    String time_stamp;
    
    String msg;

    int transmission_time;
    int time_sec;
    int time_min;
    int time_hr;

    String time_sec_msg;
    String time_min_msg;
    String time_hr_msg;

    unsigned long sampling_current_time;
    unsigned long sampling_prev_time;

    bool first_dive_flag;
    bool first_stop_flag;
    bool second_dive_flag;
    bool second_stop_flag;
    bool rise_flag;

    bool reset_enable;

    int state;

#endif 