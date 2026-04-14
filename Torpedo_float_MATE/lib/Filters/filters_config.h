#ifndef FILTERS_CONFIG_H
    #define FILTERS_CONFIG_H

    #include <math.h>
    #include <Arduino.h>

    #define GREEN   "\x1b[32m"
    
    #define Q_DEFAULT 0.001
    #define R_DEFAULT 0.02
    #define P_DEFAULT 1.0
    #define X_DEFAULT 0.0

    #define DRIFT_THRESHOLD_DEFAULT 2.0

    #ifndef ENABLE_LOGGING

        #define ENABLE_LOGGING 1
    
    #endif
    
    #if ENABLE_LOGGING
        
        #ifdef PLATFORM_PC
            #define LOG_INFO(fmt, ...) printf(GREEN"[INFO] " fmt "\n", __VA_ARGS__)

        #else
            #define LOG_INFO(fmt, ...) Serial.printf("[INFO] " fmt "\n", __VA_ARGS__)
        
        #endif

    #else
        #define LOG_INFO(...)
    
    #endif

#endif