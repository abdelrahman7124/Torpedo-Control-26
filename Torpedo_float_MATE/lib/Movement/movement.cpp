#include "movement.h"

Movement::Movement()
{
    this->power_up = 0;
    this->power_down = 0;
    this->current_depth = 0.00;
    this->target_depth = 0.00;
    this->prev_target_depth = 0.00;
    this->time = 0.00;
    this->prev_time = 0.00;
    this->dt = 0.00;
    this->hold_flag = false;
    this->dive_flag = false;
    this->rise_flag = false;
}

int Movement::dive(float depth)
{
    this->target_depth = depth;
    
    if(this->target_depth != this->prev_target_depth)
    {
        this->dive_flag = false;
    }

    if(!this->dive_flag)
    {
        this->pid.set_goal(this->target_depth);
        this->prev_target_depth = this->target_depth;
        this->dive_flag = true;
    }
    
    
    this->time = millis();
    
    if(this->prev_time == 0)
    {
        this->prev_time = this->time;
        return DIVING;
    }

    this->current_depth = this->bmp.readDepth();
    this->pid.set_reading(this->current_depth);
    
    this->dt = (this->time - this->prev_time)/MILLISECOND_IN_SECOND;
    this->pid.set_dt(this->dt);
    this->prev_time = this->time;

    double output = this->pid.run();
    
    if(output > DEPTH_THRESHOLD)
    {
        this->control.setPower_down(output);
        this->control.down();
        return DIVING;
    }

    else if (output < -DEPTH_THRESHOLD)
    {
    this->control.setPower_up(abs(output));
    this->control.up();
    return SURFACING;
    }

    else
    {
        this->control.stop();
        return IDLE;
    }
}

int Movement::hold(float time)
{   
    this->time = millis();

    if(!this->hold_flag) 
    {
        this->prev_time = this->time;
        this->hold_flag = true;
    }

    this->dt = (this->time - this->prev_time)/MILLISECOND_IN_SECOND;
    
    if(this->dt < time)
    {
        control.hover();
        return HOLDING;
    }

    this->hold_flag = false;
    this->dt = 0.00;
    return IDLE;
}

int Movement::rise_to_surface()
{
    if(!this->rise_flag)
    {
        this->target_depth = SEA_LEVEL_DEPTH;
        this->pid.set_goal(this->target_depth);
        this->rise_flag = true;
    }
    
    this->current_depth = this->bmp.readDepth();
    
    this->time = millis();
    
    if(this->prev_time == 0)
    {
        this->prev_time = this->time;
        return SURFACING;
    }
    
    this->dt = (this->time - this->prev_time)/MILLISECOND_IN_SECOND;

    this->pid.set_reading(this->current_depth);
    this->pid.set_dt(this->dt);

    this->prev_time = this->time;

    double output = this->pid.run();

    if (output < -SURFACE_THRESHOLD)
    {
        this->control.setPower_up(abs(output));
        this->control.up();
        return SURFACING;
    }

    else
    {
        this->control.stop();
        this->rise_flag = false;
        return IDLE;
    }
}