import rclpy
from rclpy.node import Node
from std_msgs.msg import String
import json

class ThrustersMixer(Node):
    def __init__(self):
        super().__init__('thrusters-mixer')

        self.cmd_sub = self.create_subscription(String, 'rov_commands', self.cmd_callback, 10)
        self.esp_pub = self.create_publisher(String, 'esp_commands', 10)
        self.get_logger().info("Thrusters Mixer Node Initialized")

    def cmd_callback(self, msg):
        try:
            data = json.loads(msg.data)
            surge = data.get('surge', 0.0)
            sway = data.get('sway', 0.0)
            heave = data.get('heave', 0.0)
            yaw = data.get('yaw', 0.0)

            Thruster1 = surge + sway + yaw      #Front Right
            Thruster2 = surge - sway - yaw      #Front Left    
            Thruster3 = surge - sway + yaw      #Back Left
            Thruster4 = surge + sway - yaw      #Back Right
            Thruster5 = heave                   #Vertical Left
            Thruster6 = heave                   #Vertical Right    

            outputs = [Thruster1, Thruster2, Thruster3, Thruster4, Thruster5, Thruster6]
            max_output = max(abs(output) for output in outputs)
            if max_output > 1.0:
                outputs = [output / max_output for output in outputs]

            pwm = [int(1500 + output * 400) for output in outputs]
            pwm = [max(1100, min(1900, p)) for p in pwm]

            esp_msg = String()
            esp_msg.data = json.dumps({
                'thruster_1': pwm[0],
                'thruster_2': pwm[1],
                'thruster_3': pwm[2],
                'thruster_4': pwm[3],
                'thruster_5': pwm[4],
                'thruster_6': pwm[5],
            })
            self.esp_pub.publish(esp_msg)

        except json.JSONDecodeError as e:
            self.get_logger().error(f"❗ JSON decode error in cmd_callback: {e}")

def main(args=None):
    rclpy.init(args=args)
    node = ThrustersMixer()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        rclpy.shutdown()

if __name__ == '__main__':
    main()