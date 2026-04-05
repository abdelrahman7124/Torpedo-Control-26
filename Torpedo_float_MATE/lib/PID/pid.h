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