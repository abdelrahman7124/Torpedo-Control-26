#include <Arduino.h>
#include "imu.h"
#include "ethernet.h"
unsigned long previousMillis = 0;

float angleX = 0, angleY = 0, angleZ = 0;
float threshold = 0.5;



void setup() {
    Serial.begin(9600); 
    delay(1000);
    if(ethernet_setup(IPAddress(192,168,1,30), IPAddress(192,168,1,2), 8888, 5) == 2){
        Serial.println("Ethernet initialized successfully.");
    } else {
        Serial.println("Ethernet initialization failed!");
    }
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
    MaintainEthernet();
    imu_update();

    get_angles(angleX, angleY, angleZ, threshold);
    if (millis() - previousMillis > 500) {
        float dataArray[3] = {angleX, angleY, angleZ};
        sendDataArrayFloat(dataArray, 3);
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