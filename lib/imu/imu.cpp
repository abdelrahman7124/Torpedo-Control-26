#include <Arduino.h>
#include "Wire.h"
#include <MPU6050_light.h>

MPU6050 mpu(Wire);

float stableAngleZ = 0.0;
unsigned long lastMicros = 0;

bool imu_setup() {
    Wire.begin(21, 22); // SDA, SCL pins for ESP32
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

    lastMicros = micros();
    stableAngleZ = 0.0;

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

void get_angles(float& angleX, float& angleY, float& angleZ, float threshold) {
    angleX = mpu.getAngleX();
    angleY = mpu.getAngleY();
    // angleZ = mpu.getAngleZ();
    unsigned long currentMicros = micros();
    float dt = (currentMicros - lastMicros) / 1000000.0;
    lastMicros = currentMicros;

    float gz = mpu.getGyroZ();
    if (abs(gz) > threshold) {
        stableAngleZ += gz * dt;
    }

    angleZ = stableAngleZ;

}

void get_temperature(float& temp) {
    temp = mpu.getTemp();
}

void imu_update() {
    mpu.update();
}
void imu_updateFull(float threshold) {
    mpu.update();
    unsigned long currentMicros = micros();
    float dt = (currentMicros - lastMicros) / 1000000.0;
    lastMicros = currentMicros;

    float gz = mpu.getGyroZ();
    if (abs(gz) > threshold) {
        stableAngleZ += gz * dt;
    }
}