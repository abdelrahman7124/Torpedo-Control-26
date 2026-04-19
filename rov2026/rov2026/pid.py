class PID:
    def __init__(self, kp, ki, kd, is_angular=False):
        self.kp = kp
        self.ki = ki
        self.kd = kd
        self.is_angular = is_angular
        self.output_limits = (-1, 1)

        self.setpoint = 0.0
        self.integral = 0.0
        self.prev_error = None
    
    def reset(self):
        self.integral = 0.0
        self.prev_error = None

    def set_setpoint(self, setpoint):
        self.setpoint = setpoint
        self.integral = 0.0
        self.prev_error = None

    def set_output_limits(self, limits):
        self.output_limits = limits

    def compute(self, measurement):

        error = self.setpoint - measurement

        if self.is_angular:
            while error > 180.0:
                error -= 360.0
            while error < -180.0:
                error += 360.0

        p_term = self.kp * error

        self.integral += error
        self.integral = max(min(self.integral, 1), -1)
        i_term = self.ki * self.integral

        if self.prev_error is not None:
            d_term = self.kd * (error - self.prev_error)
        else:
            d_term = 0.0

        output = p_term + i_term + d_term
        output = max(min(output, self.output_limits[1]), self.output_limits[0])

        self.prev_error = error

        return output