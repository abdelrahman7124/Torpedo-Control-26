#ifndef THRUSTERS_H
#define THRUSTERS_H
const int NUM_THRUSTERS = 6;    

#include "imu.h"

extern IMU imu;

struct ROVCommand {
    int thrusterVals[14];
    int gripperAngle;
    int gripperOpen;
};

void parseAndDrive(char* packetBuffer);
void setupThrusters();
void setupGripper();
ROVCommand parseCommand(char* packetBuffer, float yaw);
void drive(ROVCommand cmd);
#endif