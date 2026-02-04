import rclpy
from rclpy.node import Node
from std_msgs.msg import String
import json
import math
from pid import PID

class ROVController(Node):
    def __init__(self):
        super().__init__('rov_controller')

        self.AXIS_SURGE = 1  # Forward/Backward
        self.AXIS_SWAY = 0   # Left/Right
        self.AXIS_HEAVE = 4  # Up/Down
        self.AXIS_YAW = 3    # Rotate around vertical axis

        self.deadzone = 0.1

        self.dt = 0.02  # Control loop time step

        self.YAW_RATE = math.radians(45)  # 45 degrees per second
        self.DEPTH_RATE = 0.5  # meters per second

        self.yaw_pid = PID(
            Kp=0.5,
            Ki=0.01,
            Kd=0.1,
            is_angular=True,
            output_limits=(-1.0, 1.0)
        )

        self.depth_pid = PID(
            Kp=1.0,
            Ki=0.02,
            Kd=0.1,
            is_angular=False,
            output_limits=(-1.0, 1.0)
        )

        self.current_depth = 0.0
        self.current_yaw = 0.0
        
        self.yaw_setpoint = 0.0
        self.depth_setpoint = 0.0

        self.joy_surge = 0.0
        self.joy_sway = 0.0
        self.joy_heave = 0.0
        self.joy_yaw = 0.0
        
        self.yaw_initalized = False
        self.depth_initalized = False

        self.prev_yaw_active = False
        self.prev_heave_active = False

        self.joy_sub = self.create_subscription(String, 'joy_raw', self.joy_callback, 10)
        self.imu_sub = self.create_subscription(String, 'imu_data', self.imu_callback, 10)
        self.depth_sub = self.create_subscription(String, 'depth_data', self.depth_callback, 10)
        self.cmd_pub = self.create_publisher(String, 'rov_commands', 10)

    def joy_callback(self, msg):
        try:
            data = json.loads(msg.data)
            axes = data.get('axes', [])
            if len(axes) > max(self.AXIS_SURGE, self.AXIS_SWAY, self.AXIS_HEAVE, self.AXIS_YAW):
                self.joy_surge = axes[self.AXIS_SURGE]
                self.joy_sway = axes[self.AXIS_SWAY]
                self.joy_heave = axes[self.AXIS_HEAVE]
                self.joy_yaw = axes[self.AXIS_YAW]

        except json.JSONDecodeError as e:
            self.get_logger().error(f"❗ JSON decode error in joy_callback: {e}")
    
    def imu_callback(self, msg):
        try:
            data = json.loads(msg.data)
            self.current_yaw = data.get('yaw', 0.0)
            
            if not self.yaw_initalized:
                self.yaw_setpoint = self.current_yaw
                self.yaw_pid.set_setpoint(self.yaw_setpoint)
                self.yaw_initalized = True
                self.get_logger().info(f"Yaw initialized at {math.degrees(self.current_yaw):.1f}°")

        except json.JSONDecodeError as e:
            self.get_logger().error(f"❗ JSON decode error in imu_callback: {e}")


    def depth_callback(self, msg):
        self.current_depth = float(msg.data)

        if not self.depth_initalized:
            self.depth_setpoint = self.current_depth
            self.depth_pid.set_setpoint(self.depth_setpoint)
            self.depth_initalized = True
            self.get_logger().info(f"Depth initialized at {self.current_depth:.2f}m")

    
    def control_loop(self):
        
        if not (self.yaw_initalized and self.depth_initalized):
            self.get_logger().warning("❗ Waiting for initial yaw and depth data...")
            return

        yaw_active = abs(self.joy_yaw) > self.deadzone
        heave_active = abs(self.joy_heave) > self.deadzone

        yaw_cmd = self.compute_yaw(yaw_active)
        heave_cmd = self.compute_depth(heave_active)

        surge_cmd = self.joy_surge if abs(self.joy_surge) > self.deadzone else 0.0
        sway_cmd = self.joy_sway if abs(self.joy_sway) > self.deadzone else 0.0

        cmd = {
            'surge': surge_cmd,
            'sway': sway_cmd,
            'heave': heave_cmd,
            'yaw': yaw_cmd
        }

        msg_out = String()
        msg_out.data = json.dumps(cmd)
        self.cmd_pub.publish(msg_out)

        self.prev_yaw_active = yaw_active
        self.prev_heave_active = heave_active

    def compute_yaw(self, yaw_active):
        if yaw_active:
            delta = self.joy_yaw * self.YAW_RATE * self.dt
            self.yaw_setpoint += delta

            while self.yaw_setpoint > math.pi:
                self.yaw_setpoint -= 2 * math.pi
            while self.yaw_setpoint < -math.pi:
                self.yaw_setpoint += 2 * math.pi

            self.yaw_pid.set_setpoint(self.yaw_setpoint)

        else:
            if self.prev_yaw_active:
                self.yaw_setpoint = self.current_yaw
                self.yaw_pid.set_setpoint(self.yaw_setpoint)
                self.yaw_pid.reset()

                self.get_logger().info(f"Yaw hold at {math.degrees(self.yaw_setpoint):.1f}°")
        
        return self.yaw_pid.compute(self.current_yaw)
    
    def compute_depth(self, heave_active):
        
        if heave_active:
            delta = -self.joy_heave * self.DEPTH_RATE * self.dt
            self.depth_setpoint += delta
            self.depth_setpoint = max(min(self.depth_setpoint, 50), 0.0)
            self.depth_pid.set_setpoint(self.depth_setpoint)

        else:
            if self.prev_heave_active:
                self.depth_setpoint = self.current_depth
                self.depth_pid.set_setpoint(self.depth_setpoint)
                self.depth_pid.reset()

                self.get_logger().info(f"Depth hold at {self.depth_setpoint:.2f}m")

        return self.depth_pid.compute(self.current_depth)
    
def main(args=None):
    rclpy.init(args=args)
    node = ROVController()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        rclpy.shutdown()


if __name__ == '__main__':
    main()
        