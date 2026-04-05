import rclpy
from rclpy.node import Node
from std_msgs.msg import String, Int32MultiArray
import json

class ThrustersFactors(Node):
    def __init__(self):
        super().__init__('thrusters_factors')

        
        self.factors = [1,1,1,1,1,1]

        self.create_subscription(Int32MultiArray, 'pwm_values', self.pwm_callback, 10)
        self.thruster_pub = self.create_publisher(Int32MultiArray, 'thruster_cmd', 10)

        self.get_logger().info("Factors Node Started")

    def pwm_callback(self, msg):

        pwm = [msg.data[i] * self.factors[i] for i in range(6)]

        esp_msg = String()
        esp_msg.data = f"{pwm[0]},{pwm[1]},{pwm[2]},{pwm[3]},{pwm[4]},{pwm[5]}"

        self.esp_cmd.publish(esp_msg)
        
def main(args=None):
    rclpy.init(args=args)
    node = ThrustersFactors()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()  