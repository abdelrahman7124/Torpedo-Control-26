#include <Arduino.h>
#include <stdlib.h>
#include <string.h>
#include <ESP32Servo.h>
#include "filters.h"
#include "ethernet.h"
#include "thrusters.h"
#include "pid.h"
#include "imu.h"
#include "ethernet.h"


PID pid_yaw;
PID pid_drive;
bool pid_start_flag = false;



Servo thrusters[NUM_THRUSTERS];
Servo gripperServo;

const int thrusterPins[NUM_THRUSTERS] = {26, 27, 32, 4, 25, 16};
const int gripperServoPin = 17;
const int gripperPin = 33;
unsigned long prev_time = 0;
unsigned long previousMillis = 0;

/*
Thruster Pins:
16:  Vertical Back
25:  Vertical Front
Horizontal
27:  Front left
26: Front right
4:  back right
32: back left
*/


void setupThrusters() {
    for (int i = 0; i < NUM_THRUSTERS; i++) {
        thrusters[i].attach(thrusterPins[i], 1000, 2000);
        thrusters[i].writeMicroseconds(1500);
    }
    delay(2000);

}

void setupGripper() {
    pinMode(gripperPin, OUTPUT);
    digitalWrite(gripperPin, LOW);

    gripperServo.attach(gripperServoPin, 1000, 2000);
    gripperServo.write(90);
}

void parse(char* buffer){

}



ROVCommand parseCommand(char* packetBuffer,float yaw) {
    ROVCommand cmd;
    // defaults
    for (int i = 0; i < NUM_THRUSTERS; i++) cmd.thrusterVals[i] = 1500;
    cmd.gripperAngle = 90;
    cmd.gripperOpen = 0;

    if (packetBuffer == NULL) return cmd;

    int index = 0;
    char* token = strtok(packetBuffer, ",");

    // thrusters
    
    while (token != NULL && index < 14) 
    {
        if(index < NUM_THRUSTERS)
        {
            cmd.thrusterVals[index] = constrain(atoi(token), 1100, 1900);
        }
        else
        {
            cmd.thrusterVals[index] = atoi(token);
        }
        index++;
        token = strtok(NULL, ",");
    }


    
    
    
    if(cmd.thrusterVals[12] < 0.5 && cmd.thrusterVals[13] < 0.5)
    {
        float alpha = 0.8f;
        
        if(!pid_start_flag)
        {
            pid_drive.set_goal(yaw);
            prev_time = micros(); 
            pid_start_flag = true;
        }
        
        else
        {
            pid_drive.set_dt((micros() - prev_time) * 1e-6f);
            prev_time = micros();
            pid_drive.set_reading(yaw);
            double pid_output = pid_drive.run();

            cmd.thrusterVals[0] += (alpha) * pid_output;
            cmd.thrusterVals[3] += (alpha) * pid_output;
            cmd.thrusterVals[1] -= (alpha) * pid_output;
            cmd.thrusterVals[2] -= (alpha) * pid_output;
                        
        }
    }
    
    else
    {
        pid_start_flag = false;
        pid_drive.set_goal(yaw);
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

void drive(ROVCommand cmd) 
{    
    
    for(int i = 0; i < NUM_THRUSTERS; i++)
    {
        cmd.thrusterVals[i] = constrain(cmd.thrusterVals[i], 1100, 1900);
    }
    
    for (int i = 0; i < NUM_THRUSTERS; i++) 
    {
        thrusters[i].writeMicroseconds(cmd.thrusterVals[i]);
    }
    
    gripperServo.write(cmd.gripperAngle);
    digitalWrite(gripperPin, cmd.gripperOpen ? HIGH : LOW);
    
    //Serial.print(" Pressure: ");
    // Serial.print(bmp.readPressure());
    // Serial.print(" Pa | Depth: ");
    // Serial.print(depth);
    // Serial.println(" m");
    
    // if (micros() - previousMillis > 125)
    // {
    //     float dataArray[7] =
    //     {
    //         roll,
    //         pitch,
    //         yaw,
    //         depth,
    //         pid_yaw.get_kp(),
    //         pid_yaw.get_ki(),
    //         pid_yaw.get_kd()
    //     };

    //     sendDataArrayFloat(dataArray, 7);

    //     previousMillis = micros();
    // }

    delay(100);
}