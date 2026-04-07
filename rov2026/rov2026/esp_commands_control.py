import rclpy
from rclpy.node import Node
from std_msgs.msg import String, Int32, Int32MultiArray

class EspCommandsControl(Node):
    def __init__(self):
        super().__init__('esp_commands_control')

        self.thruster_pwm = [1500, 1500, 1500, 1500, 1500, 1500]
        self.gripper_value = 0

        self.create_subscription(Int32MultiArray, 'thruster_cmd', self.thruster_callback, 10)
        #self.create_subscription(Int32, 'gripper_cmd', self.gripper_callback, 10)

        self.esp_pub = self.create_publisher(String, 'esp_commands', 10)

        self.get_logger().info("Esp Commands Control Node Started")

    def thruster_callback(self, msg):

        self.thruster_pwm = list(msg.data[:6])
        self.publish_combined()

    """def gripper_callback(self, msg):

        self.gripper_value = msg.data
        self.publish_combined()
"""
    def publish_combined(self):
        
        thrusters = self.thruster_pwm

        #gripper = self.gripper_value

        values = thrusters

        esp_msg = String()
        esp_msg.data = ','.join(str(v) for v in values)
        self.esp_pub.publish(esp_msg)

def main(args=None):
    rclpy.init(args=args)
    node = EspCommandsControl()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()