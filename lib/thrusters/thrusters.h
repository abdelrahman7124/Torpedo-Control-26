#ifndef THRUSTERS_H
#define THRUSTERS_H
const int NUM_THRUSTERS = 6;    

struct ROVCommand {
    int thrusterVals[NUM_THRUSTERS];
    int gripperAngle;
    int gripperOpen;
};

void parseAndDrive(char* packetBuffer);
void setupThrusters();
void setupGripper();
ROVCommand parseCommand(char* packetBuffer);
void drive(ROVCommand cmd);
#endif