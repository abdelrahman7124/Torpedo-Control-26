#ifndef CALIBRATE_H
    #define CALIBRATE_H
    #include "movement.h"
    #include "calibrate_configure.h"

    enum CalibrationState
    {   
        CALIBRATING = 0,
        CALIBRATED = 1,
    };

    
    class calibrate
    {
        private:
            Movement calibration_action;
            float kp_value;
            float ki_value;
            float kd_value;

            float prev_kp_value;
            float prev_ki_value;
            float prev_kd_value;

            bool kp_convergence_flag;
            bool ki_convergence_flag;
            bool kd_convergence_flag;
        
        public:
            calibrate();
            CalibrationState tune();

    };



#endif