#ifndef RTC_CONFIG_H
    #define RTC_CONFIG_H

    #define CHIP_ENABLE_PIN 15
    #define DATA_PIN 13
    #define SCLK_PIN 12

    #define DEFAULT_ADDRESS 0x00

    #define SECONDS_ADDRESS 0x80
    #define MINUTES_ADDRESS 0x82
    #define HOURS_ADDRESS 0x84

    #define READ_CMD  0x01
    #define WRITE_CMD 0x00

    #define SCLK_DELAY 5
    #define CE_DELAY 2
#endif