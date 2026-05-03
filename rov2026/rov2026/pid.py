import time

class PID:
    def __init__(self, kp, ki, kd, is_angular=False, d_filter_alpha=0.8, antiwindup=True):
        self.kp = kp
        self.ki = ki
        self.kd = kd
        self.is_angular = is_angular
        self.alpha = d_filter_alpha
        self.antiwindup = antiwindup
        self.output_limits = (-1, 1)

        self.setpoint = 0.0
        self.integral = 0.0
        self.prev_measurement = None
        self.prev_time = None
        self.d_filtered = 0.0
    
    def reset(self):
        self.integral = 0.0
        self.prev_measurement = None
        self.prev_time = None
        self.d_filtered = 0.0

    def set_setpoint(self, setpoint):
        self.setpoint = setpoint
        self.prev_measurement = None
        self.prev_time = None
        self.d_filtered = 0.0

    def set_output_limits(self, limits):
        self.output_limits = limits

    def compute(self, measurement, dt):

        
        if dt is None:
            now = time.monotonic()
            if self.prev_time is None:
                self.prev_time = now
                self.prev_measurement = measurement
                return 0.0
            dt = now - self.prev_time
            self.prev_time = now

        if dt <= 0.0 or dt > 1.0:
            return 0.0
        

        error = self.setpoint - measurement

        if self.is_angular:
            error = ((error + 180.0) % 360.0) - 180.0

        p_term = self.kp * error

        if self.prev_measurement is None:
            d_raw = 0.0
        else:
            d_meas = (measurement - self.prev_measurement) / dt
            self.d_filtered = (self.alpha * self.d_filtered + (1.0 - self.alpha) * d_meas)
            d_raw = -self.d_filtered
        d_term = self.kd * d_raw

        tentative = p_term + self.ki * self.integral + d_term

        if self.antiwindup:
            saturated_high = (tentative >= self.output_limits[1]) and (error > 0)
            saturated_low = (tentative <= self.output_limits[0]) and (error < 0)
            if not (saturated_high or saturated_low):
                self.integral += error * dt
        else:
            self.integral += error * dt

        if self.ki != 0.0:
            out_range = self.output_limits[1] - self.output_limits[0]
            i_max = out_range / (2.0 * abs(self.ki))  # half the range
            self.integral = max(min(self.integral, i_max), -i_max)

        i_term = self.ki * self.integral

        output = p_term + i_term + d_term
        output = max(min(output, self.output_limits[1]), self.output_limits[0])

        self.prev_measurement = measurement

        return output