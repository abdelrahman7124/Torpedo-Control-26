#ifndef CONFIG_H
    #define CONFIG_H

    #include <Arduino.h>

    #define RED     "\x1b[31m"
    #define GREEN   "\x1b[32m"
    #define YELLOW  "\x1b[33m"


    #define KP_INITIAL 1.0
    #define KI_INITIAL 0.1
    #define KD_INITIAL 0.05

    #define KP_STEP_UP 1.005
    #define KP_STEP_DOWN 0.995 
    #define KI_STEP_UP 1.0007
    #define KI_STEP_DOWN 0.9998
    #define KD_STEP_UP 1.01
    #define KD_STEP_DOWN 0.99
    #define OVERSHOOT_SMALL_RESET 0.95
    #define OVERSHOOT_LARGE_RESET 0.5

    #define LOW_PASS_FILTER_VALUE 0.02

    #define MIN_OUTPUT_THRESHOLD -255
    #define MAX_OUTPUT_THRESHOLD 255
    #define NORMALIZTION_PARAMETER 0

    #define MEAN_THRESHOLD 2.0
    #define VARIANCE_THRESHOLD 10.0
    #define OVERSHOOT_THRESHOLD 5
    #define GOAL_THRESHOLD 10.00
    
    #define KP_MAX_THRESHOLD 10.0
    #define KI_MAX_THRESHOLD 2.0
    #define KD_MAX_THRESHOLD 1.0
    
    #define KP_MIN_THRESHOLD 0.0
    #define KI_MIN_THRESHOLD 0.0
    #define KD_MIN_THRESHOLD 0.0
    
    #define MAX_INTEGRAL_THRESHOLD 20.00
    #define MIN_INTEGRAL_THRESHOLD -20.00

    #ifndef ENABLE_LOGGING
    #define ENABLE_LOGGING 1
    #endif


    #if ENABLE_LOGGING
        #ifdef PLATFORM_PC
            #define LOG_INFO(fmt, ...) printf(GREEN"[INFO] " fmt "\n", __VA_ARGS__)
            #define LOG_WARN(fmt, ...) printf(YELLOW"[WARN] " fmt "\n", __VA_ARGS__)
            #define LOG_ERROR(fmt, ...) printf(RED"[ERROR] " fmt "\n", __VA_ARGS__)

        #else
            #define LOG_INFO(fmt, ...) Serial.printf("[INFO] " fmt "\n", __VA_ARGS__)
            #define LOG_WARN(fmt, ...) Serial.printf("[WARN] " fmt "\n", __VA_ARGS__)
            #define LOG_ERROR(fmt, ...) Serial.printf("[ERROR] " fmt "\n", __VA_ARGS__)

        #endif

    #else
        #define LOG_INFO(...)
        #define LOG_WARN(...) 
        #define LOG_ERROR(...)
    #endif

#endif
