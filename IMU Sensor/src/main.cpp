#include <Arduino.h>
#include <Wire.h>
#include "imu.h"
#include "filters.h"

#define SDA 21
#define SCL 22

IMU imu;
KALMAN kalman_filter_yaw;
KALMAN kalman_filter_pitch;
KALMAN kalman_filter_roll;

DRIFTNEGATION drift_negation_yaw;
DRIFTNEGATION drift_negation_pitch;
DRIFTNEGATION drift_negation_roll;

float roll, pitch, yaw;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  Wire.begin(SDA, SCL);
  imu.initialize();

  kalman_filter_pitch.set_R(0.2);
  kalman_filter_roll.set_R(0.2);
  
  drift_negation_pitch.set_threshold(0.2);
  drift_negation_roll.set_threshold(0.2);
}

void loop() {
  // put your main code here, to run repeatedly:
  imu.update();

  roll = drift_negation_roll.filter(imu.getRoll());
  roll = kalman_filter_roll.filter(roll);

  pitch = drift_negation_pitch.filter(imu.getPitch());
  pitch = kalman_filter_pitch.filter(pitch);
  
  yaw = drift_negation_yaw.filter(imu.getYaw());
  yaw = kalman_filter_yaw.filter(yaw);
  
  Serial.print("Roll: ");
  Serial.print(roll);
  
  Serial.print(" | Pitch: ");
  Serial.print(pitch);
  
  Serial.print(" | Yaw: ");
  Serial.println(yaw);
  delay(100);
}
