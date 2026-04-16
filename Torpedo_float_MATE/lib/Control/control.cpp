#include "control.h"

Control::Control() 
{
    pinMode(MOTOR_PIN_1, OUTPUT);
    pinMode(MOTOR_PIN_2, OUTPUT);
    pinMode(UPPER_LIMIT_SWITCH_PIN, INPUT);
    pinMode(LOWER_LIMIT_SWITCH_PIN, INPUT);
    this->goal = 0.0;
    this->reading = 0.0;
    this->balance_flag = false;
    this->control_current_time = 0;
    this->control_prev_time = 0;
    this->control_dt = 0.0;
    this->power_up = 0;
    this->power_down = 0;
}

void Control::up()
{
    this->balance_flag = false;
    if(digitalRead(UPPER_LIMIT_SWITCH_PIN) == HIGH)
    {
        analogWrite(MOTOR_PIN_1, this->power_up);
    }
    
    analogWrite(MOTOR_PIN_2, MIN_MOTOR_OUTPUT);
    LOG_INFO("Moving up with power %d", this->power_up);
}

void Control::down() 
{
    this->balance_flag = false;
    analogWrite(MOTOR_PIN_1, MIN_MOTOR_OUTPUT);
    
    if(digitalRead(LOWER_LIMIT_SWITCH_PIN) == HIGH)
    {
        analogWrite(MOTOR_PIN_2, this->power_down);
    }

    LOG_INFO("Moving down with power %d", this->power_down);
}

void Control::stop() 
{
    this->balance_flag = false;
    analogWrite(MOTOR_PIN_1, MIN_MOTOR_OUTPUT);
    analogWrite(MOTOR_PIN_2, MIN_MOTOR_OUTPUT);
    LOG_INFO("%s","Stopping motors");
}

void Control::setPower_up(int power) 
{
    power = constrain(power, MIN_MOTOR_OUTPUT, MAX_MOTOR_OUTPUT);
    this->power_up = power;
    LOG_INFO("Setting up power to %d", power);
}

void Control::setPower_down(int power) 
{
    power = constrain(power, MIN_MOTOR_OUTPUT, MAX_MOTOR_OUTPUT);
    this->power_down = power;
    LOG_INFO("Setting down power to %d", power);
}

void Control::hover() 
{   
    this->control_current_time = millis();

    if(!this->balance_flag) 
    {
        this->goal = bmp.readDepth();
        this->pid.set_goal(this->goal);
        LOG_INFO("Entering balance mode with goal depth %.2f", this->goal);

        this->control_prev_time = this->control_current_time;
        this->balance_flag = true;
    }

    this->control_dt = (this->control_current_time - this->control_prev_time)/ MILLISECOND_IN_SECOND;
    pid.set_dt(this->control_dt);
    this->control_prev_time = this->control_current_time;

    this->reading = bmp.readDepth();
    this->pid.set_reading(this->reading);
    int output = this->pid.run();

    if (output > HOVER_THRESHOLD) 
    {
        setPower_down(output);
        down();
    }

    else if (output < -HOVER_THRESHOLD) 
    {
        setPower_up(abs(output));
        up();
    } 

    else 
    {
        stop();
    }
}