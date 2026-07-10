#include <Arduino.h>
#include "ethernet.h"
#include "imu.h"
#include "bmp.h"
#include "thrusters.h"
#include "filters.h"
#include "uploadLan.h"

EthernetServer otaServer(80);
// BMP bmp;

unsigned long prevSendMillis = 0;  // Remove if unused
unsigned long lastRcvdTime = 0;
unsigned long dhcpTimer = 0;       // Fix: removed static (unnecessary at global scope) 
unsigned long otaTimer = 0;
ROVCommand cmd;

float roll, pitch, yaw, depth;
unsigned long imuTimer = 0;



IMU imu;

KALMAN kalman_filter_yaw;
KALMAN kalman_filter_pitch;
KALMAN kalman_filter_roll;
KALMAN kalman_filter_depth;

DRIFTNEGATION drift_negation_yaw;
DRIFTNEGATION drift_negation_pitch;
DRIFTNEGATION drift_negation_roll;

void setup() {
    //Serial.begin(9600);
    Wire.begin(IMU_SDA, IMU_SCL);
    Wire.setClock(800000); 
    // bmp.init();
    imu.initialize_connection();
    setupThrusters();
    setupGripper();
    delay(1000);
    ethernet_setup(IPAddress(192,168,1,44), IPAddress(192,168,1,2), 9000, 5);

    otaServer.begin();
    pinMode(LED_BUILTIN,OUTPUT);

    kalman_filter_pitch.set_R(0.2);
    kalman_filter_roll.set_R(0.2);
    drift_negation_pitch.set_threshold(0.2);
    drift_negation_roll.set_threshold(0.2);
}

void loop() {

    if (micros() - imuTimer >= 125) 
    {
        imu.update();
        Serial.print(yaw);
        Serial.print("  |  ");
        roll = drift_negation_roll.filter(imu.getRoll());
        roll = kalman_filter_roll.filter(roll);
        pitch = drift_negation_pitch.filter(imu.getPitch());
        pitch = kalman_filter_pitch.filter(pitch);
        yaw = drift_negation_yaw.filter(imu.getYaw());
        yaw = kalman_filter_yaw.filter(yaw);
        depth = kalman_filter_depth.filter(1080.0);
        Serial.println(depth);
    
        imuTimer = micros();
    }



    digitalWrite(LED_BUILTIN,LOW);
    if (millis() - otaTimer > 50) 
    {
        handleEthernetOTA(otaServer);
        otaTimer = millis();
    }
    
    if (millis() - dhcpTimer > 2000) {
        MaintainEthernet();
        dhcpTimer = millis();
    }

    char* incomingCmd = checkIncomingUDP();

    if (!checkNetworkHealth()) {
        recoverNetwork();
        otaServer.begin();
    }

    // Fix: guard parseCommand and drive against NULL
    cmd = parseCommand(incomingCmd, yaw);
    drive(cmd);
    if (incomingCmd != NULL) {
        lastRcvdTime = millis();
    } 
    else 
    {
        if (millis() - lastRcvdTime > 2000) {
            resetUDP();
            lastRcvdTime = millis();
        }
    }
}