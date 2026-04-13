#include "filters.h"

KALMAN::KALMAN()
{
    this->Q = Q_DEFAULT;
    this->R = R_DEFAULT;
    this->P = P_DEFAULT;
    this->X = X_DEFAULT;   
}


float KALMAN::filter(float reading)
{
    this->P = this->P + this->Q;
    this->K = this->P / (this->P + this->R);
    this->X = this->X + this->K * (reading - this->X);
    this->P = (1 - this->K) * this->P;
    
    return this->X;
}

void KALMAN::set_Q(float q)
{
    this->Q = q;
    LOG_INFO("Kalman filter Q set to %.5f", q);
}

void KALMAN::set_R(float r)
{
    this->R = r;
    LOG_INFO("Kalman filter R set to %.5f", r);
}

void KALMAN::set_P(float p)
{
    this->P = p;
    LOG_INFO("Kalman filter P set to %.5f", p);
}


void KALMAN::set_X(float x)
{
    this->X = x;
    LOG_INFO("Kalman filter X set to %.5f", x);
}

float KALMAN::get_Q()
{
    return this->Q;
}

float KALMAN::get_R()
{
    return this->R;
}

float KALMAN::get_P()
{
    return this->P;
}

float KALMAN::get_X()
{
    return this->X;
}


DRIFTNEGATION::DRIFTNEGATION()
{
    this->threshold = DRIFT_THRESHOLD_DEFAULT;
    this->prev_reading = 0.0;
    this->output = 0.0;
}

float DRIFTNEGATION::filter(float reading)
{
    this->difference = reading - this->prev_reading;

    if (abs(this->difference) > this->threshold)
        {
            this->output += this->difference;
        }

    this->prev_reading = reading;

    return this->output;
}

void DRIFTNEGATION::set_threshold(float t)
{
    this->threshold = t;
    LOG_INFO("Drift negation filter threshold set to %.5f", t);
}

float DRIFTNEGATION::get_threshold()
{
    return this->threshold;
}