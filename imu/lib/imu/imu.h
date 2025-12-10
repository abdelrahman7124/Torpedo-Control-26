#ifndef IMU_H
#define IMU_H

bool imu_setup();  
void get_acceleration(float& ax, float& ay, float& az);
void get_gyroscope(float& gx, float& gy, float& gz);
void get_angles(float& angleX, float& angleY, float& angleZ);
void get_temperature(float& temp);
void set_drift_threshold(float threshold);
void imu_update();

#endif