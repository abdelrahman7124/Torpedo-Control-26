#ifndef CONFIG_H
    #define CONFIG_H

    #include <Arduino.h>

    #define GREEN   "\x1b[32m"
    #define RED     "\x1b[31m"

    
    #define IMU_DEFAULT_ADDRESS 0x68
    

    
    #ifndef ENABLE_LOGGING
        
        #define ENABLE_LOGGING 1
    
    #endif

    #if ENABLE_LOGGING
        
        #ifdef PLATFORM_PC
            
            #define LOG_INFO(fmt, ...) printf(GREEN"[INFO] " fmt "\n", __VA_ARGS__)
            #define LOG_ERROR(fmt, ...) printf(RED"[ERROR] " fmt "\n", __VA_ARGS__)

        #else
            
            #define LOG_INFO(fmt, ...) Serial.printf("[INFO] " fmt "\n", __VA_ARGS__)
            #define LOG_ERROR(fmt, ...) Serial.printf("[ERROR] " fmt "\n", __VA_ARGS__)

        #endif

    #else
        
        #define LOG_INFO(...)
        #define LOG_ERROR(...)
    
    #endif


#endif