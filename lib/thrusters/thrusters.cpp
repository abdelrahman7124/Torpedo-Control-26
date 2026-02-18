#include <Arduino.h>
#include <stdlib.h>
#include <string.h>
#include <Servo.h>
#include "ethernet.h"
#define TEST_MODE true
#define NUM_THRUSTERS 4
Servo thrusters[NUM_THRUSTERS];
const int thrusterPins[NUM_THRUSTERS] = {0, 1, 2, 3};
const int freq = 5000;
const int resolution = 8;
void setupThrusters() {
    if(!TEST_MODE){
        for(int i=0; i<NUM_THRUSTERS; i++) {
                thrusters[i].attach(thrusterPins[i]);
                thrusters[i].writeMicroseconds(1500);
        }
    }else{
        ledcSetup(0, freq, resolution); ledcAttachPin(26, 0);
        ledcSetup(1, freq, resolution); ledcAttachPin(32, 1);
        ledcSetup(2, freq, resolution); ledcAttachPin(33, 2);
        ledcSetup(3, freq, resolution); ledcAttachPin(25, 3);
        ledcWrite(0, 255);//up
        ledcWrite(1, 255);//right
        ledcWrite(2, 255);//down
        ledcWrite(3, 255);//left
    }
    
}
void parseAndDrive(char* packetBuffer) {
    int thrusterIndex = 0;
    char* token = strtok(packetBuffer, ",");
    while (token != NULL && thrusterIndex < NUM_THRUSTERS) {
        int val = atoi(token); 
        
        if (TEST_MODE) {
            int brightness = atoi(token);
            ledcWrite(thrusterIndex, brightness);
        } else {
            val = constrain(val, 1100, 1900);
            thrusters[thrusterIndex].writeMicroseconds(val);
        }
        
        thrusterIndex++;
        token = strtok(NULL, ",");
    }
}
