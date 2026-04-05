#ifndef IMU_H
#define IMU_H

#include "config.h"


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