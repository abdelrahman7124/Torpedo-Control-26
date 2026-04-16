import rclpy
from rclpy.node import Node
from std_msgs.msg import String, Float32
import json
from .pid import PID


class ROVController(Node):
    def __init__(self):
        super().__init__('controller')

        self.yaw_pid = PID(kp=0.5, ki=0.01, kd=0.1, is_angular=True)
        self.depth_pid = PID(kp=1.0, ki=0.02, kd=0.1, is_angular=False)
        self.pitch_pid = PID(kp=0.5, ki=0.01, kd=0.1, is_angular=True)

        self.initialized = False

        self.joy_fb = 0.0
        self.joy_rl = 0.0
        self.joy_ud = 0.0
        self.joy_yaw = 0.0
        self.joy_active = False

        self.prev_yaw_active = False
        self.prev_ud_active = False
        
        self.current_yaw = 0.0
        self.current_depth = 0.0
        self.current_pitch = 0.0

        self.target_yaw = 0.0
        self.target_depth = 0.0

        self.YAW_RATE = 2.0
        self.DEPTH_RATE = 0.02

        self.speed_factor = 1.0

        self.create_subscription(String, 'joy_processed', self.joy_callback, 10)
        self.create_subscription(String, 'rov_telemetry', self.telemetry_callback, 10)
        self.create_subscription(Float32, 'speed_factor', self.speed_callback, 10)
        self.cmd_pub = self.create_publisher(String, 'rov_commands', 10)

        self.get_logger().info("✅ Controller started")

    def speed_callback(self, msg):
        self.speed_factor = msg.data
        self.yaw_pid.set_output_limits((-self.speed_factor, self.speed_factor))
        self.depth_pid.set_output_limits((-self.speed_factor, self.speed_factor))

    def joy_callback(self, msg):
        try:
            data = json.loads(msg.data)
            self.joy_fb = data.get('fb', 0.0)
            self.joy_rl = data.get('rl', 0.0)
            self.joy_ud = data.get('ud', 0.0)
            self.joy_yaw = data.get('yaw', 0.0)
            self.joy_active = data.get('is_active', False)
            
        except json.JSONDecodeError:
            pass

    def telemetry_callback(self, msg):
        try:
            data = json.loads(msg.data)
            self.current_yaw = data.get('yaw', 0.0)
            self.current_depth = data.get('depth', 0.0)
            self.current_pitch = data.get('pitch', 0.0)

            if not self.initialized:
                self.target_yaw = self.current_yaw
                self.target_depth = self.current_depth
                self.yaw_pid.set_setpoint(self.target_yaw)
                self.depth_pid.set_setpoint(self.target_depth)
                self.pitch_pid.set_setpoint(0.0)
                self.yaw_pid.set_output_limits((-1.0, 1.0))
                self.depth_pid.set_output_limits((-1.0, 1.0))
                self.pitch_pid.set_output_limits((-1.0, 1.0))
                self.initialized = True
                self.get_logger().info(
                    f"✅ Initialized - Yaw: {self.target_yaw:.1f}°, "
                    f"Depth: {self.target_depth:.2f}m"
                )
                return
            
            yaw_active = True if abs(self.joy_yaw) > 0.0 else False
            ud_active = True if abs(self.joy_ud) > 0.0 else False

            yaw_cmd = self.compute_yaw(yaw_active)
            ud_cmd = self.compute_depth(ud_active)
            pitch_cmd = self.pitch_pid.compute(self.current_pitch)

            self.prev_yaw_active = yaw_active
            self.prev_ud_active = ud_active

            cmd = {
                'fb': self.joy_fb,
                'rl': self.joy_rl,
                'ud': ud_cmd,
                'yaw': yaw_cmd,
                'pitch': pitch_cmd
            }
            
            self.cmd_pub.publish(String(data=json.dumps(cmd)))

        except json.JSONDecodeError:
            pass

    def compute_yaw(self, active):
        
        if active:
            pass
            # self.target_yaw += self.joy_yaw * self.YAW_RATE
            # while self.target_yaw > 180.0:
            #     self.target_yaw -= 360.0
            # while self.target_yaw < -180.0:
            #     self.target_yaw += 360.0
            # self.yaw_pid.set_setpoint(self.target_yaw)
            
        
        elif not active and self.prev_yaw_active:
            self.target_yaw = self.current_yaw
            self.yaw_pid.set_setpoint(self.target_yaw)
            self.yaw_pid.reset()

        return self.yaw_pid.compute(self.current_yaw)
    
    def compute_depth(self, active):
        if active:
            pass
            # self.target_depth += self.joy_ud * self.DEPTH_RATE
            # self.depth_pid.set_setpoint(self.target_depth)
        
        elif not active and self.prev_ud_active:
            self.target_depth = self.current_depth
            self.depth_pid.set_setpoint(self.target_depth)
            self.depth_pid.reset()

        return self.depth_pid.compute(self.current_depth)
        
def main(args=None):
    rclpy.init(args=args)
    node = ROVController()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()