#include "pid.h"

PID::PID()
{
    this->kp = KP_INITIAL;
    this->ki = KI_INITIAL;
    this->kd = KD_INITIAL;
    this->error = 0.0;
    this->sum = 0.0;
    this->diff = 0.0;
    this->reading = 0.0;
    this->prev_error = 0.0;
    this->goal = 0.0;
    this->dt = 0.0;
}

PID::~PID()
{
    LOG_INFO("%s", "Object deleted successfully");
}

void PID::set_kp(float parameter)
{
    this->kp = parameter;
    LOG_INFO("%s", "Kp parameter set");
}

void PID::set_ki(float parameter)
{
    this->ki = parameter;
    LOG_INFO("%s", "Ki parameter set");
}

void PID::set_kd(float parameter)
{
    this->kd = parameter;
    LOG_INFO("%s", "Kd parameter set");
}

void PID::set_goal(float parameter)
{
    this->goal = parameter;
    LOG_INFO("%s", "Goal set");
}

void PID::set_reading(float parameter)
{
    this->reading = parameter;
    LOG_INFO("%s", "New reading set");
}

void PID::set_dt(float parameter)
{
    this->dt = parameter;
    LOG_INFO("%s", "dt patameter set");
}

float PID::get_kp() const
{
    return this->kp;
}

float PID::get_ki() const
{
    return this->ki;
}

float PID::get_kd() const
{
    return this->kd;
}

float PID::get_goal() const
{
    return this->goal;
}

float PID::get_dt() const
{
    return this->dt;
}

double PID::clamp(double value, double min_threshold, double max_threshold)
{
    value = min(max(min_threshold,value),max_threshold);
    return value;
}

double PID::run()
{
    this->error = this->goal - this->reading;

    if(abs(this->error) <= GOAL_THRESHOLD)
    {
        this->sum = 0.00;
        this->diff = 0.00;
        this->error = 0.00;
        this->prev_error = 0.00;
        return NORMALIZTION_PARAMETER;
    }

    
    if(this->dt <= 0)
    {
        LOG_ERROR("%s", "dt must be > 0");
        return NORMALIZTION_PARAMETER;
    }
    
    else
    {
        this->sum +=  (this->dt/2) * (this->error + this->prev_error);
        this->sum = PID::clamp(this->sum, MIN_INTEGRAL_THRESHOLD, MAX_INTEGRAL_THRESHOLD);
        
        this->diff = (this->prev_error - this->error)/this->dt;
    }

    double output = (this->kp * error) + (this->ki * sum) + (this->kd * diff) + NORMALIZTION_PARAMETER;
    this->prev_error = this->error;
    output = PID::clamp(output,MIN_OUTPUT_THRESHOLD,MAX_OUTPUT_THRESHOLD);
    PID::adapt();
    return output;
}

void PID::adapt()
{
    if(this->error > DISTANCE_THRESHOLD)
    {
        this->kp += STEP_SIZE;
    }

    else
    {
        this->kp -= STEP_SIZE;
    }

    if(this->error < DISTANCE_THRESHOLD)
    {
        this->ki += STEP_SIZE;
    }

    else
    {
        this->ki -= STEP_SIZE;
    }

    if(this->diff > DIFFERENCE_THRESHOLD)
    {
        this->kd += STEP_SIZE;
    }

    else
    {
        this->kd -= STEP_SIZE;
    }

    this->kp = PID::clamp(this->kp,KP_MIN_THRESHOLD,KP_MAX_THRESHOLD);
    this->ki = PID::clamp(this->ki,KI_MIN_THRESHOLD,KI_MAX_THRESHOLD);
    this->kd = PID::clamp(this->kd,KD_MIN_THRESHOLD,KD_MAX_THRESHOLD);
}
