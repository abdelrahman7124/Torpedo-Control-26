import rclpy
from rclpy.node import Node
from std_msgs.msg import String, Int32MultiArray
import json

class ThrustersMixer(Node):
    def __init__(self):
        super().__init__('thrusters_mixer')

        self.create_subscription(String, 'rov_commands', self.cmd_callback, 10)
        self.pwm_pub = self.create_publisher(Int32MultiArray, 'pwm_values', 10)
        self.create_subscription(String, 'move_mode', self.move_mode_callback, 10)

        self.move_mode = "normal"
        self.get_logger().info("Thrusters Mixer Node Initialized")

    def move_mode_callback(self, msg):
        self.move_mode = msg.data

    def cmd_callback(self, msg):
        try:
            data = json.loads(msg.data)
            fb = data.get('fb', 0.0)
            rl = data.get('rl', 0.0)
            ud = data.get('ud', 0.0)
            yaw = data.get('yaw', 0.0)
            pitch = data.get('pitch', 0.0)

            if self.move_mode == "rotate":
                
                Thruster_FR = 0.33      #Front Right
                Thruster_FL = 0.33      #Front Left    
                Thruster_BL = 0.66      #Back Left
                Thruster_BR = 0.66      #Back Right
                Thruster_VF = 0.0       #Vertical Front
                Thruster_VB = 0.0       #Vertical Back    

            else:

                Thruster_FR = fb + rl + yaw      #Front Right
                Thruster_FL = fb - rl - yaw      #Front Left    
                Thruster_BL = fb - rl + yaw      #Back Left
                Thruster_BR = fb + rl - yaw      #Back Right
                Thruster_VF = ud + pitch         #Vertical Front
                Thruster_VB = ud - pitch         #Vertical Back    

            outputs = [Thruster_FR, Thruster_FL, Thruster_BL, Thruster_BR, Thruster_VF, Thruster_VB]
            max_output = max(abs(output) for output in outputs)
            if max_output > 1.0:
                outputs = [output / max_output for output in outputs]

            pwm = [int(1500 + output * 400) for output in outputs]
            pwm = [max(1100, min(1900, p)) for p in pwm]

            pwm_msg = Int32MultiArray()
            pwm_msg.data = pwm
            self.pwm_pub.publish(pwm_msg)

            
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