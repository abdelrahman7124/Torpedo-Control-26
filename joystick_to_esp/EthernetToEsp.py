import rclpy
from rclpy.node import Node
from std_msgs.msg import String
import socket

class EthernetToEsp(Node):
    def __init__(self):
        super().__init__('ethernet_to_esp')
        self.subscribe = self.create_subscription(String, 'joy_move', self.read_and_publish, 10)

        self.esp_ip = '192.168.1.30'
        self.esp_port = 8888
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.settimeout(0.1)

    def read_and_publish(self, msg):
        try:
            data_msg = msg.data
            self.sock.sendto(data_msg.encode(), (self.esp_ip, self.esp_port))
            self.get_logger().info(f"📤 Sent: {data_msg.strip()}")

        except socket.timeout:
            self.get_logger().warn("⚠️ No response from ESP32 (UDP timeout)")
        except Exception as e:
            self.get_logger().error(f"❗ UDP error: {e}")   

def main(args=None):
    rclpy.init(args=args)
    node = EthernetToEsp()
    try:
        rclpy.spin(node)
    except Exception as e:
        node.get_logger().error(f"💥 Node crashed: {e}")
    finally:
        node.sock.close()
        rclpy.shutdown()

if __name__ == '__main__':
    main()