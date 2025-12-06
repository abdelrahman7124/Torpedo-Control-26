import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Joy
from std_msgs.msg import String

class JoyToTwist(Node):
    def __init__(self):
        super().__init__('joy_to_twist')
        self.subscription = self.create_subscription(Joy, 'joy', self.joy_callback, 10)
        self.publisher = self.create_publisher(String, 'joy_move', 10)
        self.get_logger().info("✅ JoyToTwist node started — move the left stick to drive the turtle!")

    def joy_callback(self, msg):
        
        data = (
            f"{msg.axes[0]:.2f},{msg.axes[1]:.2f},{msg.axes[2]:.2f},{msg.axes[3]:.2f},{msg.axes[4]:.2f},"
            f"{msg.axes[5]:.2f},{msg.axes[6]},{msg.axes[7]},"
            f"{msg.buttons[0]},{msg.buttons[1]},{msg.buttons[2]},{msg.buttons[3]},"
            f"{msg.buttons[4]},{msg.buttons[5]},{msg.buttons[6]},{msg.buttons[7]},"
            f"{msg.buttons[8]},{msg.buttons[9]},{msg.buttons[10]}"
        )

        msg_out = String()
        msg_out.data = data
        self.publisher.publish(msg_out)


def main(args=None):
    rclpy.init(args=args)
    node = JoyToTwist()
    rclpy.spin(node)
    rclpy.shutdown()

if __name__ == '__main__':
    main()