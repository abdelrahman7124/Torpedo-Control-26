import rclpy
from rclpy.node import Node
from std_msgs.msg import String
import json


class ROVInput(Node):
    def __init__(self):
        super().__init__('rov_input')

        self.AXIS_FB = 1
        self.AXIS_RL = 0
        self.AXIS_UD = 4
        self.AXIS_YAW = 3
        self.DEADZONE = 0.1

        self.create_subscription(String, 'joy_raw', self.joy_callback, 10)
        self.create_subscription(String, 'rov_telemetry', self.telemetry_callback, 10)
        
        self.joy_pub = self.create_publisher(String, 'joy_processed', 10)
        self.state_pub = self.create_publisher(String, 'rov_state', 10)

        self.get_logger().info("✅ ROVInput node started")

    def joy_callback(self, msg):
        try:
            data = json.loads(msg.data)
            axes = data.get('axes', [])

            max_idx = max(self.AXIS_FB, self.AXIS_RL, self.AXIS_UD, self.AXIS_YAW)
            if len(axes) <= max_idx:
                return

            fb = axes[self.AXIS_FB]
            rl = axes[self.AXIS_RL]
            ud = axes[self.AXIS_UD]
            yaw = axes[self.AXIS_YAW]

            is_active = (
                abs(fb) > self.DEADZONE or
                abs(rl) > self.DEADZONE or
                abs(ud) > self.DEADZONE or
                abs(yaw) > self.DEADZONE
            )

            output = {
                'fb': fb,
                'rl': rl,
                'ud': ud,
                'yaw': yaw,
                'is_active': is_active
            }
            self.joy_pub.publish(String(data=json.dumps(output)))

        except json.JSONDecodeError:
            pass

    def telemetry_callback(self, msg):
        try:
            data = json.loads(msg.data)
            
            state = {
                'yaw': data.get('yaw', 0.0),
                'depth': data.get('depth', 0.0),
                'pitch': data.get('pitch', 0.0)
            }
            self.state_pub.publish(String(data=json.dumps(state)))

        except json.JSONDecodeError:
            pass


def main(args=None):
    rclpy.init(args=args)
    node = ROVInput()
    rclpy.spin(node)
    rclpy.shutdown()


if __name__ == '__main__':
    main()