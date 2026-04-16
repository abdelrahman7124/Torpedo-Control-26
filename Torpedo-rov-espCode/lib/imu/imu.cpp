#include "imu.h"

void IMU::initialize()
{
    mpu.begin(IMU_ADDRESS);
}

void IMU::readAccelerometer()
{
    mpu.getEvent(&a, &g, &temp);
    ax = a.acceleration.x;
    ay = a.acceleration.y;
    az = a.acceleration.z;
}

void IMU::readGyroscope()
{
    gx = g.gyro.x * (180/PI);
    gy = g.gyro.y * (180/PI);
    gz = g.gyro.z * (180/PI);
}

void IMU::update()
{
    readAccelerometer();
    readGyroscope();
    now = millis();
    float dt = (now - lastTime) / 1000.0;
    lastTime = now;

    float rollAcc  = atan2(ay, az) * 180 / PI;
    float pitchAcc = atan2(-ax, sqrt(ay*ay + az*az)) * 180 / PI;

    roll  = 0.98 * (roll  + gx * dt) + 0.02 * rollAcc;
    pitch = 0.98 * (pitch + gy * dt) + 0.02 * pitchAcc;

    yaw += gz * dt;
}

float IMU::getRoll()
{
    return roll;
}

float IMU::getPitch()
{
    return pitch;
}

float IMU::getYaw()
{
    return yaw;
}