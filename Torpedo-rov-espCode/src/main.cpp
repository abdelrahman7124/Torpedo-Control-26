#include <Arduino.h>
#include "imu.h"
#include "ethernet.h"
#include "bmp.h"
#include "thrusters.h"
#include "filters.h"


IMU imu;
KALMAN kalman_filter_yaw;
KALMAN kalman_filter_pitch;
KALMAN kalman_filter_roll;

DRIFTNEGATION drift_negation_yaw;
DRIFTNEGATION drift_negation_pitch;
DRIFTNEGATION drift_negation_roll;

float roll, pitch, yaw, depth;

BMP bmp;

unsigned long previousMillis = 0;
unsigned long prevSendMillis = 0;  // Remove if unused
unsigned long lastRcvdTime = 0;
unsigned long dhcpTimer = 0;       // Fix: removed static (unnecessary at global scope)
unsigned long imuTimer = 0;        // Fix: removed static

ROVCommand cmd;

void setup() {
    bmp.init();
    imu.initialize_connection();
    setupThrusters();
    setupGripper();
    delay(1000);
    ethernet_setup(IPAddress(192,168,1,44), IPAddress(192,168,1,2), 9000, 5);
    
    kalman_filter_pitch.set_R(0.2);
    kalman_filter_roll.set_R(0.2);
    drift_negation_pitch.set_threshold(0.2);
    drift_negation_roll.set_threshold(0.2);
}

void loop() {
    if (millis() - imuTimer >= 10) {
        imu.update();
        roll = drift_negation_roll.filter(imu.getRoll());
        roll = kalman_filter_roll.filter(roll);
        pitch = drift_negation_pitch.filter(imu.getPitch());
        pitch = kalman_filter_pitch.filter(pitch);
        yaw = drift_negation_yaw.filter(imu.getYaw());
        yaw = kalman_filter_yaw.filter(yaw);
        depth = bmp.readDepth();
        imuTimer = millis();
    }

    if (millis() - dhcpTimer > 2000) {
        MaintainEthernet();
        dhcpTimer = millis();
    }

    char* incomingCmd = checkIncomingUDP();

    if (!checkNetworkHealth()) {
        recoverNetwork();
    }

    // Fix: guard parseCommand and drive against NULL
    cmd = parseCommand(incomingCmd);
    drive(cmd);
    if (incomingCmd != NULL) {
        lastRcvdTime = millis();
    } else {
        if (millis() - lastRcvdTime > 2000) {
            resetUDP();
            lastRcvdTime = millis();
        }
    }

    Serial.print(bmp.getTime());
    Serial.print(" Pressure: ");
    Serial.print(bmp.readPressure());
    Serial.print(" Pa | Depth: ");
    Serial.print(depth);
    Serial.println(" m");

    if (millis() - previousMillis > 500) {
        float dataArray[4] = {roll, pitch, yaw, depth};
        sendDataArrayFloat(dataArray, 4);
        previousMillis = millis();
    }
}