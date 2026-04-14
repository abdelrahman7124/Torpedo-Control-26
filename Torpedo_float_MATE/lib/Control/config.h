#ifndef CONFIG_H
    #define CONFIG_H

    #define GREEN   "\x1b[32m"
    #define YELLOW  "\x1b[33m"

    #define MOTOR_PIN_1  5
    #define MOTOR_PIN_2  6

    #define MAX_MOTOR_OUTPUT 255
    #define MIN_MOTOR_OUTPUT 0 
    
    #define MILLISECOND_IN_SECOND 1000.00
    
    #define HOVER_THRESHOLD 5.0

    #ifndef ENABLE_LOGGING
        #define ENABLE_LOGGING 1
    #endif

    #if ENABLE_LOGGING
        #ifdef PLATFORM_PC
            #define LOG_INFO(fmt, ...) printf(GREEN"[INFO] " fmt "\n", __VA_ARGS__)
            #define LOG_WARN(fmt, ...) printf(YELLOW"[WARN] " fmt "\n", __VA_ARGS__)

        #else
            #define LOG_INFO(fmt, ...) Serial.printf("[INFO] " fmt "\n", __VA_ARGS__)
            #define LOG_WARN(fmt, ...) Serial.printf("[WARN] " fmt "\n", __VA_ARGS__)
        #endif
    
    #else
        #define LOG_INFO(...)
        #define LOG_WARN(...) 
        #define LOG_ERROR(...)
    #endif
    
#endif
