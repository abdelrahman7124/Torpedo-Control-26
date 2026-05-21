import rclpy
from rclpy.node import Node
from std_msgs.msg import String, Float32, Bool
import json


class ROVController(Node):
    def __init__(self):
        super().__init__('controller')

        self.joy_fb = 0.0
        self.joy_rl = 0.0
        self.joy_ud = 0.0
        self.joy_yaw = 0.0
        self.joy_pitch = 0.0
        self.joy_active = False

        self.prev_yaw_active = False
        self.prev_ud_active = False
        self.prev_pitch_active = False
        

        self.speed_factor = 1


        self.STARTUP_DELAY = 0.2
        
        self.prev_horizontal_idle = True
        self.prev_vertical_idle = True
        self.vertical_hold_until = None


        self.create_subscription(String, 'joy_processed', self.joy_callback, 10)
        self.create_subscription(Float32, 'speed_factor', self.speed_callback, 10)
        self.cmd_pub = self.create_publisher(String, 'rov_commands', 10)
        
        self.get_logger().info("✅ Controller started")


    SIMULTANEOUS_WINDOW = 0.1  # seconds

    def sequence_and_publish(self, fb, rl, ud, yaw, pitch):
        now = self.get_clock().now()
        horizontal_idle = not (abs(self.joy_fb) or abs(self.joy_rl) or abs(self.joy_yaw))
        vertical_idle = not (abs(self.joy_ud) or abs(self.joy_pitch))

        horizontal_starting = self.prev_horizontal_idle and not horizontal_idle
        vertical_starting = self.prev_vertical_idle and not vertical_idle

        # Track when each axis group started
        if horizontal_starting:
            self.horizontal_start_time = now
        if vertical_starting:
            self.vertical_start_time = now

        # Delay vertical if both started within a short window
        if horizontal_starting and vertical_starting:
            self.vertical_hold_until = now + rclpy.duration.Duration(seconds=self.STARTUP_DELAY)
        elif vertical_starting and hasattr(self, 'horizontal_start_time'):
            dt = (now - self.horizontal_start_time).nanoseconds / 1e9
            if dt < self.SIMULTANEOUS_WINDOW:
                self.vertical_hold_until = now + rclpy.duration.Duration(seconds=self.STARTUP_DELAY)

        self.prev_horizontal_idle = horizontal_idle
        self.prev_vertical_idle = vertical_idle

        if self.vertical_hold_until is not None:
            if now < self.vertical_hold_until:
                ud = 0.0
                pitch = 0.0
            else:
                self.vertical_hold_until = None

        cmd = {
            'fb': fb, 'rl': rl, 'ud': ud,
            'yaw': yaw, 'pitch': pitch
        }
        self.cmd_pub.publish(String(data=json.dumps(cmd)))
        return ud, pitch

    
    def speed_callback(self, msg):
        self.speed_factor = msg.data
       
    def joy_callback(self, msg):
        try:
            data = json.loads(msg.data)
            self.joy_fb = data.get('fb', 0.0)
            self.joy_rl = data.get('rl', 0.0)
            self.joy_ud = data.get('ud', 0.0)
            self.joy_yaw = data.get('yaw', 0.0)
            self.joy_pitch = data.get('pitch', 0.0)
            self.joy_active = data.get('is_active', False)

            self.sequence_and_publish(self.joy_fb, self.joy_rl, self.joy_ud, self.joy_yaw,  self.joy_pitch)
            
        except json.JSONDecodeError:
            pass
    
def main(args=None):
    rclpy.init(args=args)
    node = ROVController()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()