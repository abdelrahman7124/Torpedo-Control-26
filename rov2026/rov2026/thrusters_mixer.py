# import rclpy
# from rclpy.node import Node
# from std_msgs.msg import String, Int32MultiArray
# import json

# class ThrustersMixer(Node):
#     def __init__(self):
#         super().__init__('thrusters_mixer')

#         self.create_subscription(String, 'rov_commands', self.cmd_callback, 10)
#         self.pwm_pub = self.create_publisher(Int32MultiArray, 'pwm_values', 10)
#         #self.create_subscription(String, 'move_mode', self.move_mode_callback, 10)
#         self.create_subscription(String, 'speed_level', self.speed_level_callback, 10)

#         self.speed_level = 'MEDIUM'
#         self.speed_ranges = {"LOW": 200, "MEDIUM": 350, "HIGH": 500}

#         self.move_mode = "normal"
#         self.get_logger().info("Thrusters Mixer Node Initialized")

    
#     def speed_level_callback(self, msg):
#         if msg.data in self.speed_ranges:
#             self.speed_level = msg.data

#     # def move_mode_callback(self, msg):
#     #     self.move_mode = msg.data

#     def cmd_callback(self, msg):
#         try:
#             data = json.loads(msg.data)
#             fb = data.get('fb', 0.0)
#             rl = data.get('rl', 0.0)
#             ud = data.get('ud', 0.0)
#             yaw = data.get('yaw', 0.0)
#             pitch = data.get('pitch', 0.0)

#             # if self.move_mode == "rotate":
                
#             #     Thruster_FR = 0.33      #Front Right
#             #     Thruster_FL = 0.33      #Front Left    
#             #     Thruster_BL = 0.66      #Back Left
#             #     Thruster_BR = 0.66      #Back Right
#             #     Thruster_VF = 0.0       #Vertical Front
#             #     Thruster_VB = 0.0       #Vertical Back    

#             # else:

#             Thruster_FR = -fb - rl - yaw      #Front Right
#             Thruster_FL = -fb + rl + yaw      #Front Left    
#             Thruster_BL = -fb - rl - yaw      #Back Left
#             Thruster_BR = fb - rl + yaw      #Back Right
#             Thruster_VF = ud - pitch         #Vertical Front
#             Thruster_VB = -ud - pitch         #Vertical Back    

#             outputs = [Thruster_FR, Thruster_FL, Thruster_BL, Thruster_BR, Thruster_VF, Thruster_VB]
#             max_output = max(abs(output) for output in outputs)
#             if max_output > 1.0:
#                 outputs = [output / max_output for output in outputs]

#             pwm = [int(1500 + output * 350) for output in outputs]
#             pwm = [max(1150, min(1850, p)) for p in pwm]

#             pwm_msg = Int32MultiArray()
#             pwm_msg.data = pwm
#             self.pwm_pub.publish(pwm_msg)

            
#         except json.JSONDecodeError as e:
#             self.get_logger().error(f"❗ JSON decode error in cmd_callback: {e}")

# def main(args=None):
#     rclpy.init(args=args)
#     node = ThrustersMixer()
#     try:
#         rclpy.spin(node)
#     except KeyboardInterrupt:
#         pass
#     finally:
#         rclpy.shutdown()

# if __name__ == '__main__':
#     main()



import rclpy
from rclpy.node import Node
from std_msgs.msg import String, Int32MultiArray
import json

SPEED_LEVELS = ['LOW', 'MEDIUM', 'HIGH']
AXES = ['fb', 'rl', 'yaw', 'ud', 'pitch']
DIRECTIONS = ['fwd', 'bwd']

# Base mixing matrix: [thruster][axis]
# Thruster order: [FR, FL, BR, BL, VF, VB]
# Axis order:     [fb, rl, yaw, ud, pitch]
BASE_MIX = [
    [-1.0, -1.0, -1.0,  0.0,  0.0],   # Front Right
    [-1.0,  1.0,  1.0,  0.0,  0.0],   # Front Left
    [-1.0, -1.0, -1.0,  0.0,  0.0],   # Back Left
    [ 1.0, -1.0,  1.0,  0.0,  0.0],   # Back Right
    [ 0.0,  0.0,  0.0,  1.0, -1.0],   # Vertical Front
    [ 0.0,  0.0,  0.0, -1.0, -1.0],   # Vertical Back
]


def default_factors():
    return {
        level: {
            axis: {
                d: [1.0] * 6
                for d in DIRECTIONS
            }
            for axis in AXES
        }
        for level in SPEED_LEVELS
    }


CONSTANT_FACTORS = default_factors()


class ThrustersMixer(Node):
    def __init__(self):
        super().__init__('thrusters_mixer')

        self.create_subscription(String, 'rov_commands', self.cmd_callback, 10)
        self.create_subscription(String, 'speed_level', self.speed_level_callback, 10)
        self.create_subscription(String, 'factors', self.factors_callback, 10)

        self.thruster_pub = self.create_publisher(Int32MultiArray, 'thruster_cmd', 10)

        self.speed_level = 'MEDIUM'
        self.factors = CONSTANT_FACTORS

        self.get_logger().info("✅ Thrusters Mixer Node Initialized")

    def speed_level_callback(self, msg):
        if msg.data in SPEED_LEVELS:
            self.speed_level = msg.data
            self.get_logger().info(f"Speed level: {self.speed_level}")

    def factors_callback(self, msg):
        try:
            new_factors = json.loads(msg.data)
            if new_factors != self.factors:
                self.factors = new_factors
                self.get_logger().info("Updated factors from GUI")
        except (json.JSONDecodeError, Exception) as e:
            self.get_logger().error(f"Bad factors message: {e}")

    def cmd_callback(self, msg):
        try:
            data = json.loads(msg.data)

            commands = {
                'fb':    data.get('fb', 0.0),
                'rl':    data.get('rl', 0.0),
                'yaw':   data.get('yaw', 0.0),
                'ud':    data.get('ud', 0.0),
                'pitch': data.get('pitch', 0.0),
            }

            level_factors = self.factors.get(self.speed_level, CONSTANT_FACTORS["MEDIUM"])

            outputs = [0.0] * 6
            for t in range(6):
                total = 0.0
                for a, axis in enumerate(AXES):
                    base = BASE_MIX[t][a]
                    cmd = commands[axis]

                    # Pick factor based on COMMAND direction, not contribution
                    if cmd >= 0:
                        factor = level_factors[axis]["fwd"][t]
                    else:
                        factor = level_factors[axis]["bwd"][t]

                    total += cmd * base * factor
                outputs[t] = total

            # Normalize
            max_output = max(abs(o) for o in outputs)
            if max_output > 1.0:
                outputs = [o / max_output for o in outputs]

            # Convert to PWM
            pwm = [int(1500 + o * 350) for o in outputs]
            pwm = [max(1100, min(1900, p)) for p in pwm]

            pwm_msg = Int32MultiArray()
            pwm_msg.data = pwm
            self.thruster_pub.publish(pwm_msg)

        except json.JSONDecodeError as e:
            self.get_logger().error(f"JSON decode error: {e}")


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