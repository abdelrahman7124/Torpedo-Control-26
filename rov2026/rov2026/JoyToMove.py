import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Joy
from std_msgs.msg import String
import json

class JoyToMove(Node):
    def __init__(self):
        super().__init__('joy_to_move')
        self.subscription = self.create_subscription(Joy, 'joy', self.joy_callback, 10)
        self.publisher = self.create_publisher(String, 'joy_raw', 10)
        self.get_logger().info("✅ JoyToMove node started")

    def joy_callback(self, msg):
        
        data = {
            'axes': list(msg.axes),
            'buttons': list(msg.buttons)
        }
        
        msg_out = String()
        msg_out.data = json.dumps(data)
        self.publisher.publish(msg_out)

        


def main(args=None):
    rclpy.init(args=args)
    node = JoyToMove()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        rclpy.shutdown()

if __name__ == '__main__':
    main()