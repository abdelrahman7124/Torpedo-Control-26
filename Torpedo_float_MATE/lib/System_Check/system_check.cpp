#include "system_check.h"

SystemCheck::SystemCheck()
{
    this->pressure = 0.00;
    this->depth = 0.00;
    this->time_stamp = "";
    this->direction_flag = false;
    this->motor_state = MOVING_UP;
}

void SystemCheck::checkSensors()
{
    bmp.init();
    this->pressure = bmp.readPressure();
    this->depth = bmp.readDepth();
    this->time_stamp = bmp.getTime();
    LOG_INFO("Sensor Check: Pressure = %.2f Pa, Depth = %.2f m, Time = %s", this->pressure, this->depth, this->time_stamp.c_str());
}

void SystemCheck::checkActuators()
{
    if(this->motor_state == STOPPING)
    {
        this->direction_flag = !this->direction_flag;
    }

    if(!direction_flag)
    {
        control.setPower_up(MAX_MOTOR_OUTPUT);
        this->motor_state = control.up();
        delay(500);
    }

    else
    {
        control.setPower_down(MAX_MOTOR_OUTPUT);
        motor_state = control.down();
        delay(500);
    }

}

void SystemCheck::checkCommunication()
{
    connection.connect_init();
    for(int i = 0; i < NUM_CHECK_LOOPS; i++)
    {
        connection.send_data("Checking connection");
        delay(TIME_FOR_CHECKUP);
    }

    connection.close();
}


void SystemCheck::checkSystem()
{
    checkCommunication();
    checkSensors();
    checkActuators();
}