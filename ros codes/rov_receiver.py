#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from std_msgs.msg import String
import socket
LISTEN_IP = "0.0.0.0"
LISTEN_PORT = 8888

class RovReceiver(Node):
    def __init__(self):
        super().__init__('rov_receiver')
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind((LISTEN_IP, LISTEN_PORT))
        self.sock.setblocking(False) 
        self.publisher_ = self.create_publisher(String, 'rov_telemetry', 10)
        self.timer = self.create_timer(0.01, self.check_udp_socket)
        self.get_logger().info(f"Receiver Started. Listening on Port {LISTEN_PORT}")

    def check_udp_socket(self):
        try:
            data, addr = self.sock.recvfrom(1024)
            if data:
                sensor_data = data.decode('utf-8').strip()
                self.get_logger().info(f"RX: {sensor_data}")
                
                msg = String()
                msg.data = sensor_data
                self.publisher_.publish(msg)
        except BlockingIOError:
            pass
        except Exception as e:
            self.get_logger().error(f"Socket Error: {e}")

def main(args=None):
    rclpy.init(args=args)
    node = RovReceiver()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.sock.close()
        node.destroy_node()
        rclpy.shutdown()
