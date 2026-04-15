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
    if (!bmp.begin())
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
