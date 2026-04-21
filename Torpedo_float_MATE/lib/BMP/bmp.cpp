#include "bmp.h"

BMP::BMP()
{
    this->pressure = 0.0;
    this->depth = 0.0;
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