import os
import json
import rclpy
from rclpy.node import Node
from std_msgs.msg import Float32MultiArray, Int32MultiArray

FACTORS_DIR = os.path.join(os.path.expanduser("~"), "rov_factors")
FACTORS_FILE = os.path.join(FACTORS_DIR, "factors.json")


class ThrustersFactors(Node):
    def __init__(self):
        super().__init__('thrusters_factors')

        self.factors = [1.0] * 6

        # Load factors from JSON on startup
        self.load_factors()

        # Subscribe to PWM values
        self.create_subscription(
            Int32MultiArray, 'pwm_values', self.pwm_callback, 10
        )

        # Subscribe to factors topic (from GUI)
        self.create_subscription(
            Float32MultiArray, 'factors', self.factors_callback, 10
        )

        # Publisher for final thruster commands
        self.thruster_pub = self.create_publisher(Int32MultiArray, 'thruster_cmd', 10)

        self.get_logger().info("Factors Node Started")
        self.get_logger().info(f"Loaded factors: {self.factors}")

    def load_factors(self):
        """Load factors from JSON file on startup."""
        try:
            with open(FACTORS_FILE, "r") as f:
                loaded = json.load(f)
                if isinstance(loaded, list) and len(loaded) == 6:
                    self.factors = [round(float(v), 2) for v in loaded]
                    self.get_logger().info(f"Loaded factors from file: {self.factors}")
                else:
                    raise ValueError("Invalid factors format")
        except (FileNotFoundError, json.JSONDecodeError, ValueError) as e:
            self.get_logger().warn(f"Could not load factors ({e}), using defaults")
            self.factors = [1.0] * 6

    def save_factors(self):
        """Save current factors to JSON file."""
        try:
            os.makedirs(FACTORS_DIR, exist_ok=True)
            tmp_file = FACTORS_FILE + ".tmp"
            with open(tmp_file, "w") as f:
                json.dump([round(v, 2) for v in self.factors], f, indent=4)
                f.flush()
                os.fsync(f.fileno())
            os.replace(tmp_file, FACTORS_FILE)
            self.get_logger().info(f"Saved factors: {self.factors}")
        except Exception as e:
            self.get_logger().error(f"ERROR saving factors: {e}")

    def factors_callback(self, msg):
        """Called when GUI publishes new factors."""
        if len(msg.data) == 6:
            new_factors = [round(float(v), 2) for v in msg.data]
            if new_factors != self.factors:
                self.factors = new_factors
                self.save_factors()
                self.get_logger().info(f"Updated factors: {self.factors}")


    def pwm_callback(self, msg):
        """Apply factors to PWM values and publish."""
        pwm = []
        for i in range(6):
            offset = msg.data[i] - 1500        # how far from stop
            adjusted = 1500 + int(offset * self.factors[i])  # scale only the offset
            pwm.append(adjusted)

        esp_msg = Int32MultiArray()
        esp_msg.data = pwm
        self.thruster_pub.publish(esp_msg)


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


if __name__ == '__main__':
    main()