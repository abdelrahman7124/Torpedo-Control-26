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
    


    struct pressure_data
    {
        float depth;
        float pressure;
        String time_stamp;
    };

    void get_bmp_readings();
    void send_msg();
    MovementState execute_mission();
    void sample_readings();

    std::queue <pressure_data> pressure_log;

    wireless connection;
    BMP bmp;
    Movement action;

    float depth;
    float pressure;
    
    String time_stamp;
    
    String msg;

    bool first_dive_flag;
    bool first_stop_flag;
    bool second_dive_flag;
    bool second_stop_flag;
    bool rise_flag;

    bool reset_enable;

    MovementState mission_state;

    unsigned long sampling_current_time;
    unsigned long sampling_prev_time;

#endif 
