import rclpy
from rclpy.node import Node
from std_msgs.msg import String
import json
import math
from .pid import PID


class ROVController(Node):
    def __init__(self):
        super().__init__('controller')

        self.yaw_pid = PID(kp=0.5, ki=0.01, kd=0.1, is_angular=True, output_limits=(-1.0, 1.0))
        self.depth_pid = PID(kp=1.0, ki=0.02, kd=0.1, is_angular=False, output_limits=(-1.0, 1.0))
        self.pitch_pid = PID(kp=0.5, ki=0.01, kd=0.1, is_angular=True, output_limits=(-1.0, 1.0))

        self.mode = 'manual'
        self.prev_active = False
        self.initialized = False

        self.joy_fb = 0.0
        self.joy_rl = 0.0
        self.joy_ud = 0.0
        self.joy_yaw = 0.0
        self.joy_active = False

        self.current_yaw = 0.0
        self.current_depth = 0.0
        self.current_pitch = 0.0

        self.create_subscription(String, 'joy_processed', self.joy_callback, 10)
        self.create_subscription(String, 'rov_state', self.state_callback, 10)
        self.cmd_pub = self.create_publisher(String, 'rov_commands', 10)

        self.get_logger().info("✅ Controller started")

    def joy_callback(self, msg):
        try:
            data = json.loads(msg.data)
            self.joy_fb = data.get('fb', 0.0)
            self.joy_rl = data.get('rl', 0.0)
            self.joy_ud = data.get('ud', 0.0)
            self.joy_yaw = data.get('yaw', 0.0)
            self.joy_active = data.get('is_active', False)

            if self.prev_active and not self.joy_active:
                self.mode = 'hold'
                self.yaw_pid.set_setpoint(self.current_yaw)
                self.depth_pid.set_setpoint(self.current_depth)
                self.pitch_pid.set_setpoint(0.0)
                self.get_logger().info(
                    f"🎯 HOLD - Yaw: {math.degrees(self.current_yaw):.1f}°, "
                    f"Depth: {self.current_depth:.2f}m"
                )

            if not self.prev_active and self.joy_active:
                self.mode = 'manual'
                self.yaw_pid.reset()
                self.depth_pid.reset()
                self.pitch_pid.reset()
                self.get_logger().info("🕹️ MANUAL")

            self.prev_active = self.joy_active

        except json.JSONDecodeError:
            pass

    def state_callback(self, msg):
        try:
            data = json.loads(msg.data)
            self.current_yaw = data.get('yaw', 0.0)
            self.current_depth = data.get('depth', 0.0)
            self.current_pitch = data.get('pitch', 0.0)

            if not self.initialized:
                self.yaw_pid.set_setpoint(self.current_yaw)
                self.depth_pid.set_setpoint(self.current_depth)
                self.pitch_pid.set_setpoint(0.0)
                self.initialized = True
                self.get_logger().info(
                    f"✅ Initialized - Yaw: {math.degrees(self.current_yaw):.1f}°, "
                    f"Depth: {self.current_depth:.2f}m"
                )
                return

            if self.mode == 'manual':
                cmd = {
                    'fb': self.joy_fb,
                    'rl': self.joy_rl,
                    'ud': self.joy_ud,
                    'yaw': self.joy_yaw,
                    'pitch': 0.0
                }
            else:
                cmd = {
                    'fb': 0.0,
                    'rl': 0.0,
                    'ud': self.depth_pid.compute(self.current_depth),
                    'yaw': self.yaw_pid.compute(self.current_yaw),
                    'pitch': self.pitch_pid.compute(self.current_pitch)
                }

            self.cmd_pub.publish(String(data=json.dumps(cmd)))

        except json.JSONDecodeError:
            pass

def main(args=None):
    rclpy.init(args=args)
    node = ROVController()
    rclpy.spin(node)
    rclpy.shutdown()


if __name__ == '__main__':
    main()