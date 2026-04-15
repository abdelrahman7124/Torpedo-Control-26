#include "movement.h"

Movement::Movement()
{
    this->power_up = 0;
    this->power_down = 0;
    this->current_depth = 0.00;
    this->target_depth = 0.00;
    this->prev_target_depth = 0.00;
    this->movement_current_time = 0.00;
    this->movement_prev_time = 0.00;
    this->movement_dt = 0.00;
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
    
    
    this->movement_current_time = millis();
    
    if(this->movement_prev_time == 0)
    {
        this->movement_prev_time = this->movement_current_time;
        return DIVING;
    }

    this->current_depth = this->bmp.readDepth();
    this->pid.set_reading(this->current_depth);
    
    this->movement_dt = (this->movement_current_time - this->movement_prev_time)/MILLISECOND_IN_SECOND;
    this->pid.set_dt(this->movement_dt);
    this->movement_prev_time = this->movement_current_time;

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

int Movement::hold(float duration)
{   
    this->movement_current_time = millis();

    if(!this->hold_flag) 
    {
        this->movement_prev_time = this->movement_current_time;
        this->hold_flag = true;
    }

    this->movement_dt = (this->movement_current_time - this->movement_prev_time)/MILLISECOND_IN_SECOND;
    
    if(this->movement_dt < duration)
    {
        control.hover();
        return HOVERING;
    }

    this->hold_flag = false;
    this->movement_dt = 0.00;
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
    
    this->movement_current_time = millis();
    
    if(this->movement_prev_time == 0)
    {
        this->movement_prev_time = this->movement_current_time;
        return SURFACING;
    }
    
    this->movement_dt = (this->movement_current_time - this->movement_prev_time)/MILLISECOND_IN_SECOND;

    this->pid.set_reading(this->current_depth);
    this->pid.set_dt(this->movement_dt);

    this->movement_prev_time = this->movement_current_time;

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