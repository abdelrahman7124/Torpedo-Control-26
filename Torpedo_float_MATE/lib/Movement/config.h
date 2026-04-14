#ifndef CONTROL_CONFIG_H
    #define CONTROL_CONFIG_H    
    #define GREEN   "\x1b[32m"
    #define YELLOW  "\x1b[33m"

    #define SEA_LEVEL_DEPTH 0.00
    #define DEPTH_THRESHOLD 2.00
    #define SURFACE_THRESHOLD 0.50

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
        #define LOG_ERROR(...)
    #endif
    
#endif