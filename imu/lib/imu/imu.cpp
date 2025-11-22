#include <Arduino.h>
#include "Wire.h"
#include <MPU6050_light.h>

MPU6050 mpu(Wire);

bool imu_setup() {
    Wire.begin();
    byte status = mpu.begin();
    
    if(status != 0) {
        Serial.println("Failed to initialize MPU6050!");
        Serial.print("Error code: ");
        Serial.println(status);
        return false;
    }
    
    Serial.println("Calculating offsets...");
    mpu.calcOffsets(true, true);
    Serial.println("Offsets calculated!");
    return true;
}

void get_acceleration(float& ax, float& ay, float& az) {
    ax = mpu.getAccX();
    ay = mpu.getAccY();
    az = mpu.getAccZ();

}

void get_gyroscope(float& gx, float& gy, float& gz) {
    gx = mpu.getGyroX();
    gy = mpu.getGyroY();
    gz = mpu.getGyroZ();

}

void get_angles(float& angleX, float& angleY, float& angleZ) {
    angleX = mpu.getAngleX();
    angleY = mpu.getAngleY();
    angleZ = mpu.getAngleZ();

}

void get_temperature(float& temp) {
    temp = mpu.getTemp();
}

void imu_update() {
    mpu.update();
}