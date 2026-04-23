#include "imu.h"

IMU::IMU()
{
    this->device_address = IMU_DEFAULT_ADDRESS;
    // this->I2C_IMU.begin(IMU_SDA,IMU_SCL);

}

void IMU::initialize_connection()
{
    this->check = mpu.begin(this->device_address);
    // this->check = mpu.begin(this->device_address, &(this->I2C_IMU)) ;
    if (!this->check) 
    {
        LOG_ERROR("%s", "Failed to find IMU sensor!");
        
        // while (1) 
        // {
        //     this->check = mpu.begin(this->device_address);
        //     delay(10);
        // }
    }
    
    LOG_INFO("%s", "IMU sensor Found!");
}

void IMU::set_device_address(char address)
{
    this->device_address = address;
    LOG_INFO("Device address set to 0x%02X", address);
}

void IMU::readAccelerometer()
{
    this->mpu.getEvent(&a, &g, &temp);
    this->ax = a.acceleration.x;
    this->ay = a.acceleration.y;
    this->az = a.acceleration.z;
}

void IMU::readGyroscope()
{
    this->gx = g.gyro.x * (180/PI);
    this->gy = g.gyro.y * (180/PI);
    this->gz = g.gyro.z * (180/PI);
}

void IMU::update()
{
    this->readAccelerometer();
    this->readGyroscope();
    this->now = millis();
    this->dt = (now - lastTime) / 1000.0;
    this->lastTime = now;

    this->rollAcc  = atan2(this->ay, this->az) * 180 / PI;
    this->pitchAcc = atan2(-this->ax, sqrt(this->ay * this->ay + this->az * this->az)) * 180 / PI;

    this->roll  = 0.98 * (this->roll  + this->gx * this->dt) + 0.02 * this->rollAcc;
    this->pitch = 0.98 * (this->pitch + this->gy * this->dt) + 0.02 * this->pitchAcc;

    this->yaw += this->gz * this->dt;
}

float IMU::getRoll()
{
    return this->roll;
}

float IMU::getPitch()
{
    return this->pitch;
}

float IMU::getYaw()
{
    return this->yaw;
}