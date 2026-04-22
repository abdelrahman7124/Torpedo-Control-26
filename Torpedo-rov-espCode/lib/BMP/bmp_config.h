#ifndef BMP_CONFIG_H
    #define BMP_CONFIG_H

    #define GREEN   "\x1b[32m"
    #define YELLOW  "\x1b[33m"

    #define BMP_SDA 13
    #define BMP_SCL 14

    #define SEA_LEVEL_PRESSURE 101325
    #define WATER_DENSITY 1000
    #define GRAVITY 9.81

    #define SECONDS_IN_MINUTE 60
    #define MINUTES_IN_HOUR 60
    #define HOURS_IN_DAY 24

    #define MILLISECOND_IN_SECOND 1000.00
    
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
    #endif



#endif