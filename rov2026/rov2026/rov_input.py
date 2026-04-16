import rclpy
from rclpy.node import Node
from std_msgs.msg import Float32, String
import json


class ROVInput(Node):
    def __init__(self):
        super().__init__('rov_input')

        self.AXIS_FB = 1
        self.AXIS_RL = 0
        self.AXIS_UD = 4
        self.AXIS_YAW = 3

        self.BTN_TOGGLE_MODE = 4
        self.BTN_LOW_SPEED = 5
        self.BTN_MED_SPEED = 6
        self.BTN_HIGH_SPEED = 7

        self.BTN_ROTATE = 8

        self.DEADZONE = 0.5

        self.speed_mode = "continuous"
        self.speed_levels = [0.33, 0.66, 1]
        self.speed_tags = ["LOW", "MEDIUM", "HIGH"]
        self.speed_index = 0    # 0: Low, 1: Medium, 2: High

        self.prev_buttons = []

        self.move_mode = "normal"


        self.create_subscription(String, 'joy_raw', self.joy_callback, 10)
        
        self.joy_pub = self.create_publisher(String, 'joy_processed', 10)
        self.speed_pub = self.create_publisher(Float32, 'speed_factor', 10)
        self.move_mode_pub = self.create_publisher(String, 'move_mode', 10)

        self.get_logger().info("✅ ROVInput node started")
    
    def is_btn_pressed(self, buttons, index):
        if not self.prev_buttons or index >= len(self.prev_buttons):
            return False
        return buttons[index] == 1 and self.prev_buttons[index] == 0
    
    def handle_speed(self, buttons):

        speed_factor = 1

        if self.is_btn_pressed(buttons, self.BTN_TOGGLE_MODE):
            if self.speed_mode == 'continuous':
                self.speed_mode = 'discrete'

            elif self.speed_mode == 'discrete':
                self.speed_mode = 'continuous'

            self.get_logger().info(f"Speed Mode: {self.speed_mode}")

        if self.speed_mode == 'discrete':
            if self.is_btn_pressed(buttons, self.BTN_LOW_SPEED):
                self.speed_index = 0
                self.get_logger().info(f"Current Speed Level: {self.speed_tags[self.speed_index]}")
            elif self.is_btn_pressed(buttons, self.BTN_MED_SPEED):
                self.speed_index = 1
                self.get_logger().info(f"Current Speed Level: {self.speed_tags[self.speed_index]}")
            elif self.is_btn_pressed(buttons, self.BTN_HIGH_SPEED):
                self.speed_index = 2
                self.get_logger().info(f"Current Speed Level: {self.speed_tags[self.speed_index]}")

            speed_factor = self.speed_levels[self.speed_index]
            
        msg = Float32()
        msg.data = float(speed_factor)
        self.speed_pub.publish(msg)

    def set_speed(self, value):
        if self.speed_mode == 'discrete':
            if abs(value) < self.DEADZONE:
                return 0.0
            speed = self.speed_levels[self.speed_index]
            return speed if value > 0.0 else -speed
        
        elif self.speed_mode == 'continuous':
            return 0.0 if abs(value) < self.DEADZONE else value
    
            
    def joy_callback(self, msg):
        try:
            data = json.loads(msg.data)
            axes = data.get('axes', [])
            buttons = data.get('buttons', [])

            max_idx = max(self.AXIS_FB, self.AXIS_RL, self.AXIS_UD, self.AXIS_YAW)
            if len(axes) <= max_idx:
                return
            
            self.handle_speed(buttons)
            

            fb_raw = axes[self.AXIS_FB]
            rl_raw = axes[self.AXIS_RL]
            ud_raw = axes[self.AXIS_UD]
            yaw_raw = axes[self.AXIS_YAW]

            fb = self.set_speed(fb_raw)
            rl = self.set_speed(rl_raw)
            ud = self.set_speed(ud_raw)
            yaw = self.set_speed(yaw_raw)

            is_active = (
                abs(fb_raw) > self.DEADZONE or
                abs(rl_raw) > self.DEADZONE or
                abs(ud_raw) > self.DEADZONE or
                abs(yaw_raw) > self.DEADZONE
            )
 
            output = {
                'fb': fb,
                'rl': rl,
                'ud': ud,
                'yaw': yaw,
                'is_active': is_active
            }
            self.joy_pub.publish(String(data=json.dumps(output)))


            if self.is_btn_pressed(buttons, self.BTN_ROTATE):
                if self.move_mode == "rotate":
                    self.move_mode = "normal"
                else:
                    self.move_mode = "rotate"
                self.get_logger().info(f"Move Mode: {self.move_mode}")
            
                self.move_mode_pub.publish(String(data=self.move_mode))   

            self.prev_buttons = buttons  
            

        except json.JSONDecodeError:
            pass

def main(args=None):
    rclpy.init(args=args)
    node = ROVInput()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()