#include <Arduino.h>
#include "ethernet.h"
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

void setup() {
    Serial.begin(9600);
    Wire.begin(IMU_SDA, IMU_SCL);
    // bmp.init();
    imu.initialize_connection();
    setupThrusters();
    setupGripper();
    delay(1000);
    ethernet_setup(IPAddress(192,168,1,44), IPAddress(192,168,1,2), 9000, 5);

    otaServer.begin();
    pinMode(LED_BUILTIN,OUTPUT);
}

void loop() {
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
    cmd = parseCommand(incomingCmd);
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