import rclpy
from rclpy.node import Node
from std_msgs.msg import String, Int32MultiArray
import json

class GripperController(Node):
    def __init__(self):
        super().__init__('gripper_controller')

        self.BTN_GRIPPER_TOGGLE = 0
        self.BTN_GRIPPER_SERVO_RIGHT = 1
        self.BTN_GRIPPER_SERVO_LEFT = 3

        self.gripper_value = 0
        self.servo_value = 0
    
        self.prev_buttons = []


        self.create_subscription(String, 'joy_raw', self.joy_callback, 10)
        self.gripper_pub = self.create_publisher(Int32MultiArray, 'gripper_cmd', 10)

        self.get_logger().info("Gripper Node Started")
    
    def joy_callback(self, msg):

        try:
            data = json.loads(msg.data)
            buttons = data.get('buttons', [])
            
            if buttons[self.BTN_GRIPPER_TOGGLE] == 1 and (not self.prev_buttons or self.prev_buttons[self.BTN_GRIPPER_TOGGLE] == 0):
                self.get_logger().info("Gripper TOGGLE")
                self.gripper_value = 1 - self.gripper_value

            if buttons[self.BTN_GRIPPER_SERVO_RIGHT] == 1:
                self.get_logger().info("Gripper SERVO RIGHT")
                self.servo_value = 180
            elif buttons[self.BTN_GRIPPER_SERVO_LEFT] == 1:
                self.get_logger().info("Gripper SERVO LEFT")
                self.servo_value = 0
            else:
                self.servo_value = 90

            self.prev_buttons = buttons


            gripper_msg = Int32MultiArray()
            gripper_msg.data = [self.servo_value, self.gripper_value]
            self.gripper_pub.publish(gripper_msg)

        except json.JSONDecodeError:
            self.get_logger().error("Failed to decode JSON from joy_raw message")



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
