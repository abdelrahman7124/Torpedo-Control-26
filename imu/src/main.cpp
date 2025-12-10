#include <Arduino.h>
#include "imu.h"

unsigned long previousMillis = 0;

float angleX = 0, angleY = 0, angleZ = 0;

void setup() {
    Serial.begin(9600); 
    delay(1000);

    Serial.println("Initializing IMU...");
    Serial.println("KEEP SENSOR STATIONARY FOR CALIBRATION...");
    
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

    get_angles(angleX, angleY, angleZ);

    if (millis() - previousMillis > 500) {
        
        Serial.print("Pitch(X): ");
        Serial.print(angleX, 1);
        
        Serial.print(" | Roll(Y): ");
        Serial.print(angleY, 1);
        
        Serial.print(" | Yaw(Z): ");
        Serial.println(angleZ, 2);
        
        previousMillis = millis();
    }

    delay(5);
    
}