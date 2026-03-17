#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from std_msgs.msg import String
import socket
ESP_IP = "192.168.1.55"
ESP_PORT = 9000

class RosToEsp(Node):
    def __init__(self):
        super().__init__('ros_to_esp')
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.esp_addr = (ESP_IP, ESP_PORT)
        self.current_cmd = "1500,1500,1500,1500,1500,1500" 
        self.subscription = self.create_subscription(
            String, 'esp_commands', self.command_callback, 10)
        self.timer = self.create_timer(0.1, self.send_udp_packet)
        self.get_logger().info(f"Sender Started. Targeting {ESP_IP}:{ESP_PORT}")

    def command_callback(self, msg):
        self.current_cmd = msg.data

    def send_udp_packet(self):
        try:
            self.sock.sendto(self.current_cmd.encode(), self.esp_addr)
        except Exception as e:
            self.get_logger().error(f"UDP Error: {e}")

def main(args=None):
    rclpy.init(args=args)
    node = RosToEsp()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.sock.close()
        node.destroy_node()
        rclpy.shutdown()