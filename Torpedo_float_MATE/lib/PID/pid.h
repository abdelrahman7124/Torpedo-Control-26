/*
Author: Marwan Aly
Project: PID controller
Team: Torpedo ROV
Description: Handles PID controller logic and decision making for movement of an autonmous object 
*/


#ifndef PID_H
#define PID_H
#include "config.h"

class PID
{
    private:
        float kp;
        float ki;
        float kd;

        float error;
        float sum;
        float diff;
        float reading;
        float prev_error;
        float goal;
        float dt;
        
        float mean_error;
        float error_variance;
        float last_error;
        float overshoot_count;

        void update();
        void adapt();
        double clamp(double value, double min_threshold, double max_threshold);

    public:
        PID();
        ~PID();

        void set_kp(float parameter);
        void set_ki(float parameter);
        void set_kd(float parameter);
        void set_goal(float parameter);
        void set_reading(float parameter);
        void set_dt(float parameter);

        float get_kp() const;
        float get_ki() const;
        float get_kd() const;
        float get_goal() const;
        float get_dt() const;

        double run();

};

#endif