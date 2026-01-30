#include <Arduino.h>
#include "imu.h"
#include "ethernet.h"
#include "pressureSensor.h"
#include "thrusters.h"
// #include <Servo.h>
unsigned long previousMillis = 0;

float angleX = 0, angleY = 0, angleZ = 0;
float threshold = 0.5;
Pressure mySensor;
unsigned long prevSendMillis = 0;
unsigned long lastRcvdTime = 0;
// Servo esc;
void setup() {
    Serial.begin(115200);
    while (!Serial);
    mySensor.init();
    Serial.println("Sensor Initialized.");
    setupThrusters();
    Serial.println("Thrusters Initialized.");
    delay(1000);
    int data = ethernet_setup(IPAddress(192,168,1,55), IPAddress(192,168,1,2), 9000, 5);
    if(data == 2){
        Serial.println("Ethernet initialized successfully.");
    } else {
        Serial.print("--");
        Serial.print(data);
        Serial.println("--");
        Serial.println("Ethernet initialization failed!");
    }
    Serial.println("Initializing IMU...");
    Serial.println("KEEP SENSOR STATIONARY FOR CALIBRATION...");
    mySensor.depthInitialization();
    if (!imu_setup()) {
        Serial.println("IMU initialization failed! Check connections.");
        while(1) {
            delay(1000);
            Serial.println("System halted - IMU error");
        }
    }
    
    Serial.println("IMU Ready!");    
    // esc.attach(4); // أي pin PWM في ESP32
    // esc.writeMicroseconds(1500); // neutral
    // delay(3000); // arming time
}

void loop() {
    static unsigned long dhcpTimer = 0;
    if (millis() - dhcpTimer > 2000) {
        MaintainEthernet(); 
        dhcpTimer = millis();
    }
    imu_update();
    mySensor.update();
    imu_updateFull(threshold);
    char* incomingCmd = checkIncomingUDP(); 
    if(!checkNetworkHealth()){
        Serial.println("Network issue detected. Attempting to recover...");
        recoverNetwork();
    }
    if (incomingCmd != NULL) {
        lastRcvdTime = millis();
        Serial.print("---------------------------------RX: "); Serial.println(incomingCmd);
        
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
    if (millis() - lastRcvdTime > 2000) {
        for(int i=0; i<4; i++) ledcWrite(i, 0); 
    }
    // mySensor.display();
    get_angles(angleX, angleY, angleZ, threshold);
    if (millis() - previousMillis > 500) {
        Serial.print("Pressure Sensor Data:");
        Serial.println(mySensor.getDepth());
        float dataArray[4] = {angleX, angleY, angleZ,mySensor.getDepth()};
        sendDataArrayFloat(dataArray, 4);
        Serial.print("Pitch(X): ");
        Serial.print(angleX, 1);
        Serial.print(" | Roll(Y): ");
        Serial.print(angleY, 1);
        Serial.print(" | Yaw(Z): ");
        Serial.println(angleZ, 2);
        previousMillis = millis();
    }
//    if (millis() - lastRcvdTime > 10000) {
//        Serial.println("System Frozen. Rebooting...");
//        delay(100); 
//        ESP.restart();
//    }
    // esc.writeMicroseconds(1500); // neutral
    // delay(5000);
    // esc.writeMicroseconds(1700); // forward
    // delay(5000);

    // esc.writeMicroseconds(1500); // stop
    // delay(2000);

    // // esc.writeMicroseconds(1400); // reverse
    // // delay(5000);
    // esc.writeMicroseconds(1300); // reverse
    // delay(5000);
    // esc.writeMicroseconds(1500); // neutral
    // delay(5000);
    
}