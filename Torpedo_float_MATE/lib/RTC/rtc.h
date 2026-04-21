#ifndef RTC_H
    #define RTC_h

    #include "rtc_config.h"
    #include <Arduino.h>

    class RTC
    {
        private:
            uint8_t address;
            uint8_t seconds;
            uint8_t minutes;
            uint8_t hours;
            void write_data(uint8_t data);
            uint8_t read_data();
            String rtc_msg;
        
        public:
            RTC();
            void set_address(uint8_t address);
            uint8_t get_address();
            String get_timeStamp();
    };
#endif