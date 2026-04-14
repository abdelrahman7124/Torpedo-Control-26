/*
Author: Marwan Aly
Project: Filters for sensor readings
Team: Torpedo ROV
Description: Includes different types of filters to handle sensor readings 
*/

#ifndef FILTERS_H
#define FILTERS_H

#include "config.h"

class KALMAN
{
    private:
        float Q;
        float R;
        float P;
        float X;
        float K; 

    public:
        KALMAN();
        float filter(float reading);
        void set_Q(float q);
        void set_R(float r);
        void set_P(float p);
        void set_X(float x);
        float get_Q();
        float get_R();
        float get_P();
        float get_X();
};

class DRIFTNEGATION
{
    private:
        float threshold;
        float prev_reading;
        float output;
        float difference;

    public:
        DRIFTNEGATION();
        float filter(float reading);
        void set_threshold(float t);
        float get_threshold();
};


#endif