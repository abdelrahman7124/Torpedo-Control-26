# import os
# import json
# import rclpy
# from rclpy.node import Node
# from std_msgs.msg import String, Int32, Int32MultiArray

# # FACTORS_DIR = os.path.join(os.path.expanduser("~"), "rov_factors")
# # FACTORS_FILE = os.path.join(FACTORS_DIR, "factors.json")

# SPEED_LEVELS = ['LOW', 'MEDIUM', 'HIGH']


# def default_factors():
#     return {
#         str(level): {
#             "forward":  [1.0] * 6,
#             "backward": [1.0] * 6
#         }
#         for level in SPEED_LEVELS
#     }


# # ── Hardcoded constant factors ──
# # Format: { level: { "forward": [6 floats], "backward": [6 floats] } }
# # Thruster order: [Front Right, Front Left, Back Right, Back Left, Top Front, Top Back]
# CONSTANT_FACTORS = {
#     "LOW": {
#         "forward":  [0.50, 0.50, 0.50, 0.50, 0.50, 0.50],
#         "backward": [0.50, 0.50, 0.50, 0.50, 0.50, 0.50],
#     },
#     "MEDIUM": {
#         "forward":  [1.0, 1.0, 0.62, 0.75, 1.0, 1.0],
#         "backward": [1.0, 1.0, 0.69, 0.98, 1.0, 1.0],
#     },
#     "HIGH": {
#         "forward":  [1.00, 1.00, 1.00, 1.00, 1.00, 1.00],
#         "backward": [1.00, 1.00, 1.00, 1.00, 1.00, 1.00],
#     },
# }


# class ThrustersFactors(Node):
#     def __init__(self):
#         super().__init__('thrusters_factors')

#         # Use hardcoded constant factors instead of loading from file
#         self.factors = CONSTANT_FACTORS
#         self.current_level = 'MEDIUM'

#         # self.load_factors()  # ← commented out, using CONSTANT_FACTORS instead

#         # PWM input
#         self.create_subscription(
#             Int32MultiArray, 'pwm_values', self.pwm_callback, 10
#         )

#         # Factors from GUI (JSON string) — commented out since using constants
#         # self.create_subscription(
#         #     String, 'factors', self.factors_callback, 10
#         # )

#         # Speed level
#         self.create_subscription(
#             String, 'speed_level', self.level_callback, 10
#         )

#         # Final thruster commands
#         self.thruster_pub = self.create_publisher(
#             Int32MultiArray, 'thruster_cmd', 10
#         )

#         self.get_logger().info("Thruster Factors Node Started (using constant factors)")
#         self.get_logger().info(f"Factors: {json.dumps(self.factors)}")

#     # ── persistence (all commented out) ──

#     # def load_factors(self):
#     #     try:
#     #         with open(FACTORS_FILE, "r") as f:
#     #             loaded = json.load(f)
#     #             for lv in SPEED_LEVELS:
#     #                 ls = str(lv)
#     #                 if ls not in loaded:
#     #                     raise ValueError(f"Missing level {ls}")
#     #                 for d in ["forward", "backward"]:
#     #                     if d not in loaded[ls]:
#     #                         raise ValueError(f"Missing {d} in level {ls}")
#     #                     if not (isinstance(loaded[ls][d], list)
#     #                             and len(loaded[ls][d]) == 6):
#     #                         raise ValueError(f"Bad {d} array in level {ls}")
#     #             self.factors = {
#     #                 str(lv): {
#     #                     d: [round(float(v), 2) for v in loaded[str(lv)][d]]
#     #                     for d in ["forward", "backward"]
#     #                 }
#     #                 for lv in SPEED_LEVELS
#     #             }
#     #             self.get_logger().info("Loaded factors from file")
#     #     except (FileNotFoundError, json.JSONDecodeError, ValueError) as e:
#     #         self.get_logger().warn(f"Could not load factors ({e}), using defaults")
#     #         self.factors = default_factors()

#     # def save_factors(self):
#     #     try:
#     #         os.makedirs(FACTORS_DIR, exist_ok=True)
#     #         tmp_file = FACTORS_FILE + ".tmp"
#     #         with open(tmp_file, "w") as f:
#     #             json.dump(self.factors, f, indent=4)
#     #             f.flush()
#     #             os.fsync(f.fileno())
#     #         os.replace(tmp_file, FACTORS_FILE)
#     #         self.get_logger().info("Saved factors to file")
#     #     except Exception as e:
#     #         self.get_logger().error(f"ERROR saving factors: {e}")

#     # ── callbacks ──

#     def level_callback(self, msg):
#         level = msg.data
#         if level in SPEED_LEVELS:
#             self.current_level = level
#             self.get_logger().info(f"Speed level set to {level}")
#         else:
#             self.get_logger().warn(
#                 f"Unknown level '{level}', expected one of {SPEED_LEVELS}"
#             )

#     # def factors_callback(self, msg):
#     #     try:
#     #         new_factors = json.loads(msg.data)
#     #         if new_factors != self.factors:
#     #             self.factors = new_factors
#     #             self.save_factors()
#     #             self.get_logger().info("Updated factors from GUI")
#     #     except (json.JSONDecodeError, Exception) as e:
#     #         self.get_logger().error(f"Bad factors message: {e}")

#     def pwm_callback(self, msg):
#         level_str = self.current_level

#         fwd_factors = self.factors[level_str]["forward"]
#         bwd_factors = self.factors[level_str]["backward"]

#         pwm = []
#         for i in range(6):
#             offset = msg.data[i] - 1500

#             if offset <= 0:
#                 factor = fwd_factors[i]
#             else:
#                 factor = bwd_factors[i]

#             adjusted = 1500 + int(offset * factor)
#             pwm.append(adjusted)

#         esp_msg = Int32MultiArray()
#         esp_msg.data = pwm
#         self.thruster_pub.publish(esp_msg)


# def main(args=None):
#     rclpy.init(args=args)
#     node = ThrustersFactors()
#     try:
#         rclpy.spin(node)
#     except KeyboardInterrupt:
#         pass
#     finally:
#         node.destroy_node()
#         rclpy.shutdown()


# if __name__ == '__main__':
#     main()