#include <Arduino.h>
#include <stdlib.h>
#include <string.h>
#include <ESP32Servo.h>  // not <Servo.h>
#include "ethernet.h"
#define NUM_THRUSTERS 6
Servo thrusters[NUM_THRUSTERS+1];
const int thrusterPins[NUM_THRUSTERS+1] = {2, 4, 16, 25, 26, 27, 17};// 17 is for the gripper servo
const int freq = 5000;
const int resolution = 8;
void setupThrusters() {
    for(int i=0; i<NUM_THRUSTERS; i++) {
            thrusters[i].attach(thrusterPins[i]);
            thrusters[i].writeMicroseconds(1500);
    }
    thrusters[NUM_THRUSTERS].attach(thrusterPins[NUM_THRUSTERS]);
    thrusters[NUM_THRUSTERS].writeMicroseconds(1500);
    pinMode(5, OUTPUT);
    
}
void parseAndDrive(char* packetBuffer) {
    int thrusterIndex = 0;
    char* token = strtok(packetBuffer, ",");
    while (token != NULL && thrusterIndex < NUM_THRUSTERS) {
        int val = atoi(token); 
        val = constrain(val, 1100, 1900);
        thrusters[thrusterIndex].writeMicroseconds(val);
        thrusterIndex++;
        token = strtok(NULL, ",");
    }
    int val2 = 0;
    if (token != NULL) {
        val2 = atoi(token);
        thrusters[NUM_THRUSTERS].writeMicroseconds(val2);
        token = strtok(NULL, ",");
    }
    if (token != NULL) {
    val2 = atoi(token);
        if(val2==1){
            digitalWrite(5, HIGH);//gripper open
        } else {
            digitalWrite(5, LOW);//gripper close
        }
    }
}

