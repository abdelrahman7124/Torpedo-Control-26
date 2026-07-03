#ifndef THRUSTERS_H
#define THRUSTERS_H
const int NUM_THRUSTERS = 6;  
const int NUM_DIRECTIONS = 6;

#include "imu.h"

extern IMU imu;

struct ROVCommand {
    int thrusterVals[NUM_THRUSTERS];
    float directionVals[NUM_DIRECTIONS];
    int gripperAngle;
    int gripperOpen;
};

void parseAndDrive(char* packetBuffer);
void setupThrusters();
void setupGripper();
ROVCommand parseCommand(char* packetBuffer);
void drive(ROVCommand cmd);
void setValues(float kp, float ki, float kd);
#endif