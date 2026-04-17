#include "system_check.h"

SystemCheck::SystemCheck()
{
    this->pressure = 0.00;
    this->depth = 0.00;
    this->time_stamp = "";
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
    control.setPower_up(MAX_MOTOR_OUTPUT);
    control.up();
    LOG_INFO("%s", "Actuator Check: Moving up at maximum power");  
    delay(TIME_FOR_CHECKUP);
    
    control.setPower_down(MAX_MOTOR_OUTPUT);
    control.down();
    LOG_INFO("%s", "Actuator Check: Moving down at maximum power");
    delay(TIME_FOR_CHECKUP);
    control.stop();

}

void SystemCheck::checkCommunication()
{
    connection.connect_init();
    for(int i = 0; i < NUM_CHECK_LOOPS; i++)
    {
        connection.send_data("Checking connection");
        delay(TIME_FOR_CHECKUP);
    }
}


void SystemCheck::checkSystem()
{
    checkSensors();
    checkActuators();
    checkCommunication();
}