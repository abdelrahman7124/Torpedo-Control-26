#include "bmp.h"

BMP::BMP()
{
    this->pressure = 0.0;
    this->depth = 0.0;
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