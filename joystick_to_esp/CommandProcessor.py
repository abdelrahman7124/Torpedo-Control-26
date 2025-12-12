import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Joy
from std_msgs.msg import String
import json

class CommandProcessor(Node):
    def __init__(self):
        super().__init__('command_processor')
        self.subscription = self.create_subscription(String, 'joy_raw', self.process_callback, 10)
        self.publisher = self.create_publisher(String, 'esp_commands', 10)

        self.get_logger().info("✅ CommandProcessor node started")

    def process_callback(self, msg):
        try:
            data = json.loads(msg.data)
            axes = data.get('axes', [])
            buttons = data.get('buttons', [])
            
            esp_command = self.format_esp_command(axes, buttons)

            msg_out = String()
            msg_out.data = esp_command
            self.publisher.publish(msg_out)

            self.get_logger().info(f"Processed command: {esp_command}")

        except json.JSONDecodeError as e:
            self.get_logger().error(f"❗ JSON decode error: {e}")
        except Exception as e:
            self.get_logger().error(f"❗ Processing error: {e}")

    def format_esp_command(self, axes, buttons):
        return (
            f"{axes[0]:.2f},{axes[1]:.2f},{axes[2]:.2f},{axes[3]:.2f},"
            f"{axes[4]:.2f},{axes[5]:.2f},{int(axes[6])},{int(axes[7])},"
            f"{buttons[0]},{buttons[1]},{buttons[2]},{buttons[3]},"
            f"{buttons[4]},{buttons[5]},{buttons[6]},{buttons[7]},"
            f"{buttons[8]},{buttons[9]},{buttons[10]}"
        )
    
def main(args=None):
    rclpy.init(args=args)
    node = CommandProcessor()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        rclpy.shutdown()

if __name__ == '__main__':
    main()