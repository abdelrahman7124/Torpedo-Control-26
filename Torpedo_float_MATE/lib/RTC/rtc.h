#ifndef RTC_H
    #define RTC_H

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
            void readRegisters();
            void writeRegister(uint8_t address, uint8_t data);
            uint8_t bcd_to_dec(uint8_t val);
            uint8_t dec_to_bcd(uint8_t val);
            String rtc_msg;
            String sec_msg;
            String min_msg;
            String hr_msg;
            
            public:
            RTC();
            void set_time(uint8_t hours, uint8_t minutes, uint8_t seconds);
            void set_address(uint8_t address);
            uint8_t get_address();
            String get_timeStamp();
            uint8_t getSeconds();
            uint8_t getMinutes();
            uint8_t getHours();
    };
#endif