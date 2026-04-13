#include <Arduino.h>
#include "imu.h"
#include "ethernet.h"
#include "pressureSensor.h"
#include "thrusters.h"
unsigned long previousMillis = 0;
#include "filters.h"
#define SDA 21
#define SCL 22
IMU imu;
KALMAN kalman_filter_yaw;
KALMAN kalman_filter_pitch;
KALMAN kalman_filter_roll;

DRIFTNEGATION drift_negation_yaw;
DRIFTNEGATION drift_negation_pitch;
DRIFTNEGATION drift_negation_roll;

float roll, pitch, yaw;
Pressure mySensor;
unsigned long prevSendMillis = 0;
unsigned long lastRcvdTime = 0;
void setup() {
    mySensor.init();
    setupThrusters();
    delay(1000);
    int data = ethernet_setup(IPAddress(192,168,1,55), IPAddress(192,168,1,2), 9000, 5);
    mySensor.depthInitialization();
    Wire.begin(SDA, SCL);
    imu.initialize();
    kalman_filter_pitch.set_R(0.2);
    kalman_filter_roll.set_R(0.2);
    drift_negation_pitch.set_threshold(0.2);
    drift_negation_roll.set_threshold(0.2);
}

void loop() {
    imu.update();
    roll = drift_negation_roll.filter(imu.getRoll());
    roll = kalman_filter_roll.filter(roll);
    pitch = drift_negation_pitch.filter(imu.getPitch());
    pitch = kalman_filter_pitch.filter(pitch);
    yaw = drift_negation_yaw.filter(imu.getYaw());
    yaw = kalman_filter_yaw.filter(yaw);
    static unsigned long dhcpTimer = 0;
    if (millis() - dhcpTimer > 2000) {
        MaintainEthernet();
        dhcpTimer = millis();
    }
    mySensor.update();
    char* incomingCmd = checkIncomingUDP();
    if(!checkNetworkHealth()){
        Serial.println("Network issue detected. Attempting to recover...");
        recoverNetwork();
    }
    if (incomingCmd != NULL) {
        lastRcvdTime = millis();
        Serial.print("---------------------------------RX: "); Serial.println(incomingCmd);
        if(incomingCmd=="-1")ESP.restart();
        parseAndDrive(incomingCmd);
    } else {
        static unsigned long waitTimer = 0;
        if(millis() - waitTimer > 2000) {
            Serial.print("Waiting for data on Port ");
            Serial.println(9000);
            waitTimer = millis();
            resetUDP();
        }
    }
    mySensor.display();
    if (millis() - previousMillis > 500) {
        float dataArray[4] = {roll, pitch, yaw, mySensor.getDepth()};
        sendDataArrayFloat(dataArray, 4);
        previousMillis = millis();
    }
    delay(100);
}