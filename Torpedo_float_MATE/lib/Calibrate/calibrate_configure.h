#ifndef CALIBRATE_CONFIGURE_H
    #define CALIBRATE_CONFIGURE_H

    #define CALIBRATION_DEPTH 2
    #define CONVERGE_THRESHOLD 0.2

    #define KP_STORE_ADDRESS 0
    #define KI_STORE_ADDRESS 4
    #define KD_STORE_ADDRESS 8

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