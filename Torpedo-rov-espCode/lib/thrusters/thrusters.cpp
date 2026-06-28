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


PID pid;
bool pid_start_flag = false;
IMU imu;

unsigned long imuTimer = 0;

KALMAN kalman_filter_yaw;
KALMAN kalman_filter_pitch;
KALMAN kalman_filter_roll;
KALMAN kalman_filter_depth;

DRIFTNEGATION drift_negation_yaw;
DRIFTNEGATION drift_negation_pitch;
DRIFTNEGATION drift_negation_roll;

float roll, pitch, yaw, depth;

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

        
    kalman_filter_pitch.set_R(0.2);
    kalman_filter_roll.set_R(0.2);
    drift_negation_pitch.set_threshold(0.2);
    drift_negation_roll.set_threshold(0.2);
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

    if (packetBuffer == NULL) return cmd;

    int index = 0;
    char* token = strtok(packetBuffer, ",");

    // thrusters
    
    while (token != NULL && index < NUM_THRUSTERS) 
    {
        cmd.thrusterVals[index] = constrain(atoi(token), 1100, 1900);
        index++;
        token = strtok(NULL, ",");
    }
    
    if(cmd.thrusterVals[10] < 0.25 || cmd.thrusterVals[11] < 0.25)
    {
        float alpha = 0.3;
        
        if(!pid_start_flag)
        {
            pid.set_goal(yaw);
            pid_start_flag = true;
        }
        
        else
        {
            pid.set_dt((millis() - prev_time)/1000.0f);
            pid.set_reading(yaw);
            double pid_output = pid.run();

            cmd.thrusterVals[0] = (1-alpha) * cmd.thrusterVals[0] - (alpha) * pid_output;
            cmd.thrusterVals[3] = (1-alpha) * cmd.thrusterVals[3] - (alpha) * pid_output;
            cmd.thrusterVals[1] = (1-alpha) * cmd.thrusterVals[1] + (alpha) * pid_output;
            cmd.thrusterVals[2] = (1-alpha) * cmd.thrusterVals[2] + (alpha) * pid_output;

            // cmd.thrusterVals[0] -= pid_output;
            // cmd.thrusterVals[3] -= pid_output;
            // cmd.thrusterVals[1] += pid_output;
            // cmd.thrusterVals[2] += pid_output;
        }
    }
    
    else
    {
        pid_start_flag = false;
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

    prev_time = millis();
    return cmd;
}

void drive(ROVCommand cmd) 
{
    if (millis() - imuTimer >= 10) 
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

        imuTimer = millis();
    }

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

    Serial.print(" Pressure: ");
    // Serial.print(bmp.readPressure());
    Serial.print(" Pa | Depth: ");
    Serial.print(depth);
    Serial.println(" m");

    if (millis() - previousMillis > 500)
    {
        float dataArray[7] =
        {
            roll,
            pitch,
            yaw,
            depth,
            pid.get_kp(),
            pid.get_ki(),
            pid.get_kd()
        };

        sendDataArrayFloat(dataArray, 7);

        previousMillis = millis();
    }

    delay(100);
}