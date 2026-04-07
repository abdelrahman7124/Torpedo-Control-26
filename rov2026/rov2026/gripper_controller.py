"""import rclpy
from rclpy.node import Node
from std_msgs.msg import String, Int32

class GripperController(Node):
    def __init__(self):
        super().__init__('gripper_controller')

        self.BTN_OPEN = 0
        self.BTN_CLOSE = 1

        self.create_subscription(String, 'joy_raw', self.joy_callback, 10)
        self.gripper_pub = self.create_publisher(Int32, 'gripper_cmd', 10)

        self.get_logger().info("Gripper Node Started")

    def is_button_pressed(self, buttons, index):
        return buttons[index] == 1
    
    def joy_callback(self, msg):
        self.gripper_pub.publish(0)

def main(args=None):
    rclpy.init(args=args)
    node = GripperController()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()    
"""