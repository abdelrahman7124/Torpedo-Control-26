#include "bmp.h"

BMP::BMP()
{
    this->pressure = 0.0;
    this->depth = 0.0;
    this->time_sec = 0.0;
    this->time_min = 0.0;
    this->time_hr = 0.0;
    this->time = 0.0;
    this->time_sec_msg = "";
    this->time_min_msg = "";
    this->time_hr_msg = "";
    this->time_stamp = "";
}

void BMP::init()
{
    if (!bmp.begin())
    {
        LOG_WARN("%s","Could not find a valid BMP sensor");
    }
}

float BMP::readPressure()
{
    this->pressure = bmp.readPressure();
    return this->pressure;
}

float BMP::readDepth()
{
    this->pressure = this->readPressure();
    this->depth = (this->pressure - SEA_LEVEL_PRESSURE) / (WATER_DENSITY * GRAVITY);
    return this->depth;
}

String BMP::getTime()
{
    this->time = millis() / 1000.0;
    this->time_sec = this->time % SECONDS_IN_MINUTE;
    this->time_min = (this->time / SECONDS_IN_MINUTE) % MINUTES_IN_HOUR;
    this->time_hr = (this->time / (SECONDS_IN_MINUTE * MINUTES_IN_HOUR)) % HOURS_IN_DAY;
    
    if (this->time_hr < 10)
    {
        this->time_hr_msg = "0" + String(this->time_hr);
    }
    
    else
    {
        this->time_hr_msg = String(this->time_hr);
    }

    if (this->time_min < 10)
    {
        this->time_min_msg = "0" + String(this->time_min);
    }

    else
    {
        this->time_min_msg = String(this->time_min);
    }

    if (this->time_sec < 10)
    {
        this->time_sec_msg = "0" + String(this->time_sec);
    }

    else
    {
        this->time_sec_msg = String(this->time_sec);
    }

    this->time_stamp = "[" + this->time_hr_msg + ":" + this->time_min_msg + ":" + this->time_sec_msg + "]";
    
    return this->time_stamp;
}