#include "rtc.h"

RTC::RTC()
{
    this->address = DEFAULT_ADDRESS;
    this->seconds = 0;
    this->minutes = 0;
    this->hours = 0;

    this->rtc_msg = "";
    
    pinMode(CHIP_ENABLE_PIN, OUTPUT);
    pinMode(SCLK_PIN,OUTPUT);
    pinMode(DATA_PIN, OUTPUT);
}

uint8_t RTC::get_address()
{
    return this->address;
}

void RTC::set_address(uint8_t val)
{
    this->address = val;
}

void RTC::write_data(uint8_t data)
{
    pinMode(DATA_PIN, OUTPUT);

    for(int i = 0; i < 8; i++)
    {
        digitalWrite(DATA_PIN, (this->address >> i) & 1);
        digitalWrite(SCLK_PIN, HIGH);
        digitalWrite(SCLK_PIN, LOW);
    }
}

uint8_t RTC::read_data()
{
    pinMode(DATA_PIN, INPUT);
    uint8_t data = 0;
    for(int i = 0; i < 8; i++)
    {
        if(digitalRead(DATA_PIN))
        {
            data |= (1 << i); 
        }
        digitalWrite(SCLK_PIN, HIGH);
        digitalWrite(SCLK_PIN, LOW);
    }

    return data;
}



String RTC::get_timeStamp()
{
    digitalWrite(CHIP_ENABLE_PIN , HIGH);
    
    write_data(SECONDS_ADDRESS);
    this->seconds = read_data();
    
    write_data(MINUTES_ADDRESS);
    this->minutes = read_data();

    write_data(HOURS_ADDRESS);
    this->hours = read_data();

    rtc_msg = "[" + (String)this->hours + ":" + (String)this->minutes + ":" + (String)this->seconds + "]";

    return rtc_msg;
}