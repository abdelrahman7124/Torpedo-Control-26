import time
import math

class PID:
    def __init__(self, kp, ki, kd, is_angular=False, output_limits=(-1.0, 1.0)):
        self.kp = kp
        self.ki = ki
        self.kd = kd
        self.is_angular = is_angular
        self.output_limits = output_limits

        self.setpoint = 0.0
        self.integral = 0.0
        self.prev_error = 0.0
        self.prev_time = 0.0

        self.reset()
    
    def reset(self):
        self.integral = 0.0
        self.prev_error = None
        self.prev_time = None

    def set_setpoint(self, setpoint):
        self.setpoint = setpoint
        self.integral = 0.0
        self.prev_error = None

    def compute(self, measurement):

        current_time = time.time()
        if self.prev_time is None:
            dt = 0.02
        else:
            dt = current_time - self.prev_time
        dt = max(dt, 0.001)  # Prevent division by zero

        error = self.setpoint - measurement

        if self.is_angular:
            while error > math.pi:
                error -= 2 * math.pi
            while error < -math.pi:
                error += 2 * math.pi

        p_term = self.kp * error

        self.integral += error * dt
        self.integral = max(min(self.integral, 1), -1)
        i_term = self.ki * self.integral

        if self.prev_error is not None:
            d_term = self.kd * (error - self.prev_error) / dt
        else:
            d_term = 0.0

        output = p_term + i_term + d_term
        output = max(min(output, self.output_limits[1]), self.output_limits[0])

        self.prev_error = error
        self.prev_time = current_time

        return output