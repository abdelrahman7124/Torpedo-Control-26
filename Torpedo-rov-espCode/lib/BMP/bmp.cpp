#include "bmp.h"

BMP::BMP()
{
    this->pressure = 0.0;
    this->depth = 0.0;
    this->time_sec = 0;
    this->time_min = 0;
    this->time_hr = 0;
    this->bmp_time = 0;
    this->time_sec_msg = "";
    this->time_min_msg = "";
    this->time_hr_msg = "";
    this->time_stamp = "";
}

void BMP::init()
{
    I2C_BMP.begin(BMP_SDA, BMP_SCL);
    
    if (!bmp.begin(BMP085_STANDARD , &I2C_BMP))
    {
        LOG_WARN("%s","Could not find a valid BMP sensor");
    }
}

float BMP::readPressure()
{
    this->pressure = filter.filter(bmp.readPressure());
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
    this->bmp_time = millis() / MILLISECOND_IN_SECOND;
    this->time_sec = this->bmp_time % SECONDS_IN_MINUTE;
    this->time_min = (this->bmp_time / SECONDS_IN_MINUTE) % MINUTES_IN_HOUR;
    this->time_hr = (this->bmp_time / (SECONDS_IN_MINUTE * MINUTES_IN_HOUR)) % HOURS_IN_DAY;
    
    if (time_hr < 10)
    {
        time_hr_msg = "0" + String(time_hr);
    }
    
    else
    {
        time_hr_msg = String(time_hr);
    }

    if (time_min < 10)
    {
        time_min_msg = "0" + String(time_min);
    }

    else
    {
        time_min_msg = String(time_min);
    }

    if (time_sec < 10)
    {
        time_sec_msg = "0" + String(time_sec);
    }

    else
    {
        time_sec_msg = String(time_sec);
    }

    this->time_stamp = "[" + time_hr_msg + ":" + time_min_msg + ":" + time_sec_msg + "]";
    
    return time_stamp;
}