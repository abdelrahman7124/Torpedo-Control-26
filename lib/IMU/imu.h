/*
Author: Marwan Aly
Project: IMU sensor interfacing
Team: Torpedo ROV
Description: Handles interfacing and data conversion for the MPU6050 sensor 
*/

#ifndef IMU_H
#define IMU_H

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include "imu_config.h"



class IMU
{
    private:
        Adafruit_MPU6050 mpu;
        sensors_event_t a, g, temp;

        char device_address;
        float ax, ay, az;
        float gx, gy, gz;
        float roll,pitch,yaw;
        float rollAcc, pitchAcc;
        float dt;
        unsigned long now;
        unsigned long lastTime;
        bool check;
        // TwoWire I2C_IMU = TwoWire(0);
        
        void readAccelerometer();
        void readGyroscope();

    public:
        IMU();
        void set_device_address(char address);
        void initialize_connection();
        void update();
        float getRoll();
        float getPitch();
        float getYaw();
};


#endif