#ifndef CONFIG_H
    #define CONFIG_H

    #include <WiFi.h>
    #include <Arduino.h>
    
    #define GREEN   "\x1b[32m"
    #define RED     "\x1b[31m"
    #define YELLOW  "\x1b[33m"


    #define DEFAULT_SSID "Topedo_Float"
    #define DEFAULT_PASSWORD "12345678"
    #define DEFAULT_CLIENT_IP IPAddress(192,168,4,2)
    #define DEFAULT_PORT 1234
    #define PASSWORD_MIN_LENGTH 8
    #define MIN_PORT 1024
    #define MAX_PORT 65535

    #ifndef ENABLE_LOGGING
        
        #define ENABLE_LOGGING 1
    
    #endif


    #if ENABLE_LOGGING
        #ifdef PLATFORM_PC
            #define LOG_INFO(fmt, ...) printf(GREEN"[INFO] " fmt "\n", __VA_ARGS__)
            #define LOG_ERROR(fmt, ...) printf(RED"[ERROR] " fmt "\n", __VA_ARGS__)
            #define LOG_WARN(fmt, ...) printf(YELLOW"[WARN] " fmt "\n", __VA_ARGS__)


        #else
            #define LOG_INFO(fmt, ...) Serial.printf("[INFO] " fmt "\n", __VA_ARGS__)
            #define LOG_ERROR(fmt, ...) Serial.printf("[ERROR] " fmt "\n", __VA_ARGS__)
            #define LOG_WARN(fmt, ...) Serial.printf("[WARN] " fmt "\n", __VA_ARGS__)
        #endif

    #else
        #define LOG_INFO(...)
        #define LOG_ERROR(...)
        #define LOG_WARN(...)
    #endif

#endif