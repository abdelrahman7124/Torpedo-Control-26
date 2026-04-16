#include "filters.h"

KALMAN::KALMAN()
{
    Q = 0.001;
    R = 0.02;
    P = 1.0;
    X = 0.0;   
}


float KALMAN::filter(float reading)
{
    P = P + Q;
    K = P / (P + R);
    X = X + K * (reading - X);
    P = (1 - K) * P;

    return X;
}

void KALMAN::set_Q(float q)
{
    Q = q;
}

void KALMAN::set_R(float r)
{
    R = r;
}

void KALMAN::set_P(float p)
{
    P = p;
}

void KALMAN::set_X(float x)
{
    X = x;
}

float KALMAN::get_Q()
{
    return Q;
}

float KALMAN::get_R()
{
    return R;
}

float KALMAN::get_P()
{
    return P;
}

float KALMAN::get_X()
{
    return X;
}


DRIFTNEGATION::DRIFTNEGATION()
{
    threshold = 2.0;
    prev_reading = 0.0;
    output = 0.0;
}

float DRIFTNEGATION::filter(float reading)
{
    difference = reading - prev_reading;

    if (abs(difference) > threshold)
        {
            output += difference;
        }

    prev_reading = reading;

    return output;
}

void DRIFTNEGATION::set_threshold(float t)
{
    threshold = t;
}

float DRIFTNEGATION::get_threshold()
{
    return threshold;
}