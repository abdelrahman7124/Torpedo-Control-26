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
        self.joy_pitch = 0.0
        self.joy_active = False

        self.prev_yaw_active = False
        self.prev_ud_active = False
        self.prev_pitch_active = False
        
        self.current_yaw = 0.0
        self.current_depth = 0.0
        self.current_pitch = 0.0

        self.target_yaw = 0.0
        self.target_depth = 0.0
        self.target_pitch = 0.0

        self.YAW_RATE = 2.0
        self.DEPTH_RATE = 0.02

        self.speed_factor = 1.0

        

        self.create_subscription(String, 'joy_processed', self.joy_callback, 10)
        self.create_subscription(String, 'rov_telemetry', self.telemetry_callback, 10)
        self.create_subscription(Float32, 'speed_factor', self.speed_callback, 10)
        self.cmd_pub = self.create_publisher(String, 'rov_commands', 10)
        self.create_subscription(String, 'pid_gains', self.gains_callback, 10)
        self.gains_pub = self.create_publisher(String, 'pid_gains_current', 10)
        self.create_timer(1.0, self.publish_current_gains)

        self.last_telemetry_time  = self.get_clock().now()
        self.telemetry_timeout = 0.5 
        self.telemetry_timed_out = False

        self.create_timer(0.05, self.check_telemetry_timeout)
        

        self.get_logger().info("✅ Controller started")


    def gains_callback(self, msg):
        try:
            data = json.loads(msg.data)

            changed = []

            # Yaw
            if 'yaw_kp' in data and data['yaw_kp'] != self.yaw_pid.kp:
                self.yaw_pid.kp = data['yaw_kp']
                changed.append(f"yaw_kp={data['yaw_kp']:.4f}")
            if 'yaw_ki' in data and data['yaw_ki'] != self.yaw_pid.ki:
                self.yaw_pid.ki = data['yaw_ki']
                self.yaw_pid.reset()
                changed.append(f"yaw_ki={data['yaw_ki']:.4f}")
            if 'yaw_kd' in data and data['yaw_kd'] != self.yaw_pid.kd:
                self.yaw_pid.kd = data['yaw_kd']
                changed.append(f"yaw_kd={data['yaw_kd']:.4f}")

            # Depth
            if 'depth_kp' in data and data['depth_kp'] != self.depth_pid.kp:
                self.depth_pid.kp = data['depth_kp']
                changed.append(f"depth_kp={data['depth_kp']:.4f}")
            if 'depth_ki' in data and data['depth_ki'] != self.depth_pid.ki:
                self.depth_pid.ki = data['depth_ki']
                self.depth_pid.reset()
                changed.append(f"depth_ki={data['depth_ki']:.4f}")
            if 'depth_kd' in data and data['depth_kd'] != self.depth_pid.kd:
                self.depth_pid.kd = data['depth_kd']
                changed.append(f"depth_kd={data['depth_kd']:.4f}")

            # Pitch
            if 'pitch_kp' in data and data['pitch_kp'] != self.pitch_pid.kp:
                self.pitch_pid.kp = data['pitch_kp']
                changed.append(f"pitch_kp={data['pitch_kp']:.4f}")
            if 'pitch_ki' in data and data['pitch_ki'] != self.pitch_pid.ki:
                self.pitch_pid.ki = data['pitch_ki']
                self.pitch_pid.reset()
                changed.append(f"pitch_ki={data['pitch_ki']:.4f}")
            if 'pitch_kd' in data and data['pitch_kd'] != self.pitch_pid.kd:
                self.pitch_pid.kd = data['pitch_kd']
                changed.append(f"pitch_kd={data['pitch_kd']:.4f}")

            if changed:
                self.get_logger().info(f"🔧 Updated: {', '.join(changed)}")

        except json.JSONDecodeError:
            pass

    def publish_current_gains(self):
        gains = {
            'yaw_kp': self.yaw_pid.kp,
            'yaw_ki': self.yaw_pid.ki,
            'yaw_kd': self.yaw_pid.kd,
            'depth_kp': self.depth_pid.kp,
            'depth_ki': self.depth_pid.ki,
            'depth_kd': self.depth_pid.kd,
            'pitch_kp': self.pitch_pid.kp,
            'pitch_ki': self.pitch_pid.ki,
            'pitch_kd': self.pitch_pid.kd,
        }
        self.gains_pub.publish(String(data=json.dumps(gains)))


    def speed_callback(self, msg):
        self.speed_factor = msg.data
        self.yaw_pid.set_output_limits((-self.speed_factor, self.speed_factor))
        self.depth_pid.set_output_limits((-self.speed_factor, self.speed_factor))
        self.pitch_pid.set_output_limits((-self.speed_factor, self.speed_factor))

    def joy_callback(self, msg):
        try:
            data = json.loads(msg.data)
            self.joy_fb = data.get('fb', 0.0)
            self.joy_rl = data.get('rl', 0.0)
            self.joy_ud = data.get('ud', 0.0)
            self.joy_yaw = data.get('yaw', 0.0)
            self.joy_pitch = data.get('pitch', 0.0)
            self.joy_active = data.get('is_active', False)
            
        except json.JSONDecodeError:
            pass

    def check_telemetry_timeout(self):
        # if self.last_telemetry_time is None:
        #     return

        elapsed = (self.get_clock().now() - self.last_telemetry_time).nanoseconds / 1e9
        if elapsed > self.telemetry_timeout:
            if not self.telemetry_timed_out:
                self.initialized = False
                self.telemetry_timed_out = True

            cmd = {
                'fb': self.joy_fb,
                'rl': self.joy_rl,
                'ud': self.joy_ud,
                'yaw': self.joy_yaw,
                'pitch': self.joy_pitch
            }

            self.cmd_pub.publish(String(data=json.dumps(cmd)))
        else:
            self.telemetry_timed_out = False

    def telemetry_callback(self, msg):

        self.last_telemetry_time = self.get_clock().now()

        try:
            data = json.loads(msg.data)
            self.current_yaw = data.get('yaw', 0.0)
            self.current_depth = data.get('depth', 0.0)
            self.current_pitch = data.get('pitch', 0.0)


            if not self.initialized:
                self.target_yaw = self.current_yaw
                self.target_depth = self.current_depth
                self.target_pitch = self.current_pitch
                self.yaw_pid.set_setpoint(self.target_yaw)
                self.depth_pid.set_setpoint(self.target_depth)
                self.pitch_pid.set_setpoint(self.target_pitch)
                self.yaw_pid.set_output_limits((-1.0, 1.0))
                self.depth_pid.set_output_limits((-1.0, 1.0))
                self.pitch_pid.set_output_limits((-1.0, 1.0))

                self.initialized = True
                self.get_logger().info(
                    f"✅ Initialized - Yaw: {self.target_yaw:.1f}°, "
                    f"Depth: {self.target_depth:.2f}m, "
                    f"Pitch: {self.target_pitch:.1f}°"
                )
                return
            
            yaw_active = True if abs(self.joy_yaw) > 0.0 else False
            ud_active = True if abs(self.joy_ud) > 0.0 else False
            pitch_active = True if abs(self.joy_pitch) > 0.0 else False


            yaw_cmd = self.compute_yaw(yaw_active)
            ud_cmd = self.compute_depth(ud_active)
            pitch_cmd = self.compute_pitch(pitch_active)

            self.prev_yaw_active = yaw_active
            self.prev_ud_active = ud_active
            self.prev_pitch_active = pitch_active

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
            output = self.joy_yaw
            
        
        elif not active and self.prev_yaw_active:
            self.target_yaw = self.current_yaw
            self.yaw_pid.set_setpoint(self.target_yaw)
            self.yaw_pid.reset()
            output = self.yaw_pid.compute(self.current_yaw)

        else:
            output = self.yaw_pid.compute(self.current_yaw)

        return float(output)
    
    def compute_depth(self, active):
        if active:
            # self.target_depth += self.joy_ud * self.DEPTH_RATE
            # self.depth_pid.set_setpoint(self.target_depth)
            output = self.joy_ud
        
        elif not active and self.prev_ud_active:
            self.target_depth = self.current_depth
            self.depth_pid.set_setpoint(self.target_depth)
            self.depth_pid.reset()
            output = self.depth_pid.compute(self.current_depth)
        else:
            output = self.depth_pid.compute(self.current_depth)

        return float(output)
    
    def compute_pitch(self, active):
        if active:
            output = self.joy_pitch
        
        elif not active and self.prev_pitch_active:
            self.target_pitch = self.current_pitch
            self.pitch_pid.set_setpoint(self.target_pitch)
            self.pitch_pid.reset()
            output = self.pitch_pid.compute(self.current_pitch)
        else:
            output = self.pitch_pid.compute(self.current_pitch)

        return float(output)
    
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