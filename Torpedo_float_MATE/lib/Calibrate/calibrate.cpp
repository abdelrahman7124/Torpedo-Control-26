#include <EEPROM.h>
#include "calibrate.h"

calibrate::calibrate()
{
    this->prev_kp_value = KP_INITIAL;
    this->prev_ki_value = KI_INITIAL;
    this->prev_kd_value = KD_INITIAL;
    this->kp_convergence_flag = false;
    this->ki_convergence_flag = false;
    this->kd_convergence_flag = false;
}


CalibrationState calibrate::tune()
{
    calibration_action.dive(CALIBRATION_DEPTH);
    calibration_action.rise_to_surface();
    
    this->kp_value = calibration_action.control.pid.get_kp();
    this->ki_value = calibration_action.control.pid.get_ki();
    this->kd_value = calibration_action.control.pid.get_kd();

    this->kp_convergence_flag = (fabs(this->kp_value - this->prev_kp_value) <= CONVERGE_THRESHOLD);
    this->ki_convergence_flag = (fabs(this->ki_value - this->prev_ki_value) <= CONVERGE_THRESHOLD);
    this->kd_convergence_flag = (fabs(this->kd_value - this->prev_kd_value) <= CONVERGE_THRESHOLD);

    if(this->kp_convergence_flag && this->ki_convergence_flag && this->kd_convergence_flag)
    {
        EEPROM.put(KP_STORE_ADDRESS, this->kp_value);
        EEPROM.put(KI_STORE_ADDRESS, this->ki_value);
        EEPROM.put(KD_STORE_ADDRESS, this->kd_value);
        EEPROM.commit();
        return CALIBRATED;
    }

    else
    {
        this->prev_kp_value = this->kp_value;
        this->prev_ki_value = this->ki_value;
        this->prev_kd_value = this->kd_value;
        return CALIBRATING;
    }
}