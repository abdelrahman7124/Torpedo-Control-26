#include "rtc.h"

RTC::RTC()
{
    this->address = DEFAULT_ADDRESS;
    this->seconds = 0;
    this->minutes = 0;
    this->hours = 0;

    this->rtc_msg = "";
    this->sec_msg = "";
    this->min_msg = "";
    this->hr_msg = "";
    
    pinMode(CHIP_ENABLE_PIN, OUTPUT);
    pinMode(SCLK_PIN,OUTPUT);
    pinMode(DATA_PIN, OUTPUT);

    pinMode(DATA_PIN, OUTPUT);
    digitalWrite(CHIP_ENABLE_PIN, HIGH);
    delayMicroseconds(CE_DELAY);
    write_data(0x8E);
    write_data(0x00);
    digitalWrite(CHIP_ENABLE_PIN, LOW);
    delayMicroseconds(CE_DELAY);
}

uint8_t RTC::get_address()
{
    return this->address;
}

void RTC::set_address(uint8_t val)
{
    this->address = val;
}

uint8_t RTC::bcd_to_dec(uint8_t val)
{
    return ((val >> 4) * 10) + (val & 0x0F);
}

uint8_t RTC::dec_to_bcd(uint8_t val)
{
    return ((val / 10) << 4) | (val % 10);
}

void RTC::write_data(uint8_t data)
{
    for(int i = 0; i < 8; i++)
    {
        digitalWrite(DATA_PIN, (data >> i) & 1);

        digitalWrite(SCLK_PIN, HIGH);
        delayMicroseconds(SCLK_DELAY);

        digitalWrite(SCLK_PIN, LOW);
        delayMicroseconds(SCLK_DELAY);

    }
}

uint8_t RTC::read_data()
{
    uint8_t data = 0;

    for(int i = 0; i < 8; i++)
    {
        
        if(digitalRead(DATA_PIN))
        {
            data |= (1 << i); 
        }

        digitalWrite(SCLK_PIN, HIGH);
        delayMicroseconds(SCLK_DELAY);
    
        digitalWrite(SCLK_PIN, LOW);
        delayMicroseconds(SCLK_DELAY);
    }

    return data;
}

void RTC::set_time(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
    pinMode(DATA_PIN, OUTPUT);
    writeRegister(SECONDS_ADDRESS, dec_to_bcd(seconds) & 0x7F);
    writeRegister(MINUTES_ADDRESS, dec_to_bcd(minutes));
    writeRegister(HOURS_ADDRESS,   dec_to_bcd(hours) & 0x3F);
}

void RTC::writeRegister(uint8_t address, uint8_t data)
{
    digitalWrite(CHIP_ENABLE_PIN, HIGH);
    delayMicroseconds(CE_DELAY);
    write_data(address & ~READ_CMD);
    write_data(data);
    digitalWrite(CHIP_ENABLE_PIN, LOW);
    delayMicroseconds(CE_DELAY);
}

void RTC::readRegisters()
{
    digitalWrite(CHIP_ENABLE_PIN, HIGH);
    delayMicroseconds(CE_DELAY);
    pinMode(DATA_PIN, OUTPUT);
    write_data(0xBF);
    
    pinMode(DATA_PIN, INPUT_PULLUP);

    this->seconds = bcd_to_dec(read_data() & 0x7F);
    this->minutes = bcd_to_dec(read_data());
    this->hours   = bcd_to_dec(read_data() & 0x3F);
    read_data();
    read_data();
    read_data();
    read_data();  
    read_data(); 

    digitalWrite(CHIP_ENABLE_PIN, LOW);
    delayMicroseconds(CE_DELAY);

}

String RTC::get_timeStamp()
{
    readRegisters();
    
    if (this->seconds < 10)
    {
        this->sec_msg = "0" + String(this->seconds);
    }
    else
    {
        this->sec_msg = String(this->seconds);
    }

    if (this->minutes < 10)
    {
        this->min_msg = "0" + String(this->minutes);
    }
    else
    {
        this->min_msg = String(this->minutes);
    }

    if (this->hours < 10)
    {
        this->hr_msg = "0" + String(this->hours);
    }
    else
    {
        this->hr_msg = String(this->hours);
    }

    rtc_msg = "[" + this->hr_msg + ":" + this->min_msg + ":" + this->sec_msg + "]";

    return rtc_msg;
}

uint8_t RTC::getSeconds()
{
    return this->seconds;
}

uint8_t RTC::getMinutes()
{
    return this->minutes;
}

uint8_t RTC::getHours()
{
    return this->hours;
}
