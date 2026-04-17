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
    this->mean_error = 0.0;
    this->error_variance = 0.0;
    this->last_error = 0.0;
    this->overshoot_count = 0.0;
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
    this->sum = 0.00;
    this->prev_error = 0.00;
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
        
        this->diff = (this->error - this->prev_error)/this->dt;
    }

    double output = (this->kp * error) + (this->ki * sum) + (this->kd * diff) + NORMALIZTION_PARAMETER;
    this->prev_error = this->error;
    output = PID::clamp(output,MIN_OUTPUT_THRESHOLD,MAX_OUTPUT_THRESHOLD);
    PID::adapt();
    return output;
}

void PID::update()
{
    this->mean_error = (1 - LOW_PASS_FILTER_VALUE) * this->mean_error + LOW_PASS_FILTER_VALUE * this->error;
    
    float deviation = this->error - this->mean_error;
    this->error_variance = (1 - LOW_PASS_FILTER_VALUE) * this->error_variance + LOW_PASS_FILTER_VALUE * deviation * deviation;

    if ((((this->error) > 0) && ((this->last_error) < 0) ) || (((this->error) < 0) && ((this->last_error) > 0))) 
    {
        if(abs(this->error) > GOAL_THRESHOLD)
        {
            this->overshoot_count++;
        }
    }
    this->last_error = this->error; 
}

void PID::adapt()
{
    update();

    if(abs(this->mean_error) > MEAN_THRESHOLD)
    {
        this->ki *= KI_STEP_UP;
    }
    else
    {
        this->ki *= KI_STEP_DOWN;
    }


    if(this->error_variance > VARIANCE_THRESHOLD)
    {
        this->kp *= KP_STEP_DOWN;
        this->kd *= KD_STEP_UP;
    }

    else
    {
        this->kp *= KP_STEP_UP;
        this->kd *= KD_STEP_DOWN;
    }

    if(this->overshoot_count > OVERSHOOT_THRESHOLD)
    {
        this->kp *= KP_STEP_DOWN;
        this->kd *= KD_STEP_UP;
        this->overshoot_count *= OVERSHOOT_SMALL_RESET;
    }

    else
    {
        this->overshoot_count *= OVERSHOOT_LARGE_RESET;
    }



    this->kp = PID::clamp(this->kp,KP_MIN_THRESHOLD,KP_MAX_THRESHOLD);
    this->ki = PID::clamp(this->ki,KI_MIN_THRESHOLD,KI_MAX_THRESHOLD);
    this->kd = PID::clamp(this->kd,KD_MIN_THRESHOLD,KD_MAX_THRESHOLD);
}
