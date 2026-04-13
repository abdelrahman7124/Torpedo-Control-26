#ifndef IMU_H
#define IMU_H

#include <Arduino.h>
#include <Adafruit_MPU6050.h>

#define IMU_ADDRESS 0x68



class IMU
{
    private:
        Adafruit_MPU6050 mpu;
        sensors_event_t a, g, temp;
        
        float ax, ay, az;
        float gx, gy, gz;
        float roll,pitch,yaw;
        unsigned long now;
        unsigned long lastTime;
        
        void readAccelerometer();
        void readGyroscope();

    public:
        void initialize();
        void update();
        float getRoll();
        float getPitch();
        float getYaw();
};


#endif