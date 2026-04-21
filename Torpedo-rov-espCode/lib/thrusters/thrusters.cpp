#include <Arduino.h>
#include <stdlib.h>
#include <string.h>
#include <ESP32Servo.h>
#include "ethernet.h"
#include "thrusters.h"


Servo thrusters[NUM_THRUSTERS];
Servo gripperServo;

const int thrusterPins[NUM_THRUSTERS] = {16, 25, 27, 26, 4, 32};
const int gripperServoPin = 17;
const int gripperPin = 33;

void setupThrusters() {
    for (int i = 0; i < NUM_THRUSTERS; i++) {
        thrusters[i].attach(thrusterPins[i], 1000, 2000);
        thrusters[i].writeMicroseconds(1500);
        delay(2000);
    }
}

void setupGripper() {
    pinMode(gripperPin, OUTPUT);
    digitalWrite(gripperPin, LOW);

    gripperServo.attach(gripperServoPin, 1000, 2000);
    gripperServo.write(90);
}

void parse(char* buffer){

}



ROVCommand parseCommand(char* packetBuffer) {
    ROVCommand cmd;
    // defaults
    for (int i = 0; i < NUM_THRUSTERS; i++) cmd.thrusterVals[i] = 1500;
    cmd.gripperAngle = 90;
    cmd.gripperOpen = 0;

    int index = 0;
    char* token = strtok(packetBuffer, ",");

    // thrusters
    while (token != NULL && index < NUM_THRUSTERS) {
        cmd.thrusterVals[index] = constrain(atoi(token), 1100, 1900);
        index++;
        token = strtok(NULL, ",");
    }

    // gripper servo
    if (token != NULL) {
        cmd.gripperAngle = constrain(atoi(token), 0, 180);
        token = strtok(NULL, ",");
    }

    // gripper open/close
    if (token != NULL) {
        cmd.gripperOpen = atoi(token) == 1 ? 1 : 0;
    }

    return cmd;
}

void drive(ROVCommand cmd) {
    for (int i = 0; i < NUM_THRUSTERS; i++) {
        thrusters[i].writeMicroseconds(cmd.thrusterVals[i]);
    }
    gripperServo.write(cmd.gripperAngle);
    digitalWrite(gripperPin, cmd.gripperOpen ? HIGH : LOW);
}