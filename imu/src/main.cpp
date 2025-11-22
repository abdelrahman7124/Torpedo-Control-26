#include <Arduino.h>
#include "imu.h"
unsigned long previousMillis = 0;
void setup() {
    Serial.begin(9600); 
    delay(1000);

    Serial.println("Initializing IMU...");
    
    if (!imu_setup()) {
        Serial.println("IMU initialization failed! Check connections.");
        while(1) {
            delay(1000);
            Serial.println("System halted - IMU error");
        }
    }
    
    Serial.println("IMU Ready!");
}

void loop() {
    imu_update();

    float ax = 0, ay = 0, az = 0;
    float gx = 0, gy = 0, gz = 0;
    float angleX = 0, angleY = 0, angleZ = 0;
    float temp = 0;    


    get_angles(angleX, angleY, angleZ);
    if (millis() - previousMillis > 1000) {

    Serial.println("==== IMU Readings ====");


    Serial.print("Angle: ");
    Serial.print(angleX, 2); Serial.print(", ");
    Serial.print(angleY, 2); Serial.print(", ");
    Serial.println(angleZ, 2);

    Serial.println("======================");
    Serial.println();

    previousMillis = millis();
    }

}