#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from std_msgs.msg import String
import socket
import json

LISTEN_IP = "0.0.0.0"
LISTEN_PORT = 8888

class EspToRos(Node):
    def __init__(self):
        super().__init__('esp_to_ros')
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind((LISTEN_IP, LISTEN_PORT))
        self.sock.setblocking(False) 
        self.telemetry = self.create_publisher(String, 'rov_telemetry', 10)
        self.timer = self.create_timer(0.01, self.check_udp_socket)
        self.get_logger().info(f"Receiver Started. Listening on Port {LISTEN_PORT}")

    def check_udp_socket(self):
        try:
            data, addr = self.sock.recvfrom(1024)

            if data:
                raw_text = data.decode('utf-8').strip()
                try:
                    values = raw_text.split(',')
                    telemetry_dict = {
                        "roll": float(values[0]),
                        "pitch": float(values[1]),
                        "yaw": float(values[2]),
                        "depth": float(values[3])
                    }
                    msg = String()
                    msg.data = json.dumps(telemetry_dict)
                    self.telemetry.publish(msg)

                except (ValueError, IndexError) as parse_error:
                    self.get_logger().warn(f"Failed to parse data '{raw_text}': {parse_error}")

        except BlockingIOError:
            pass
        except Exception as e:
            pass


def main(args=None):
    rclpy.init(args=args)
    node = EspToRos()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.sock.close()
        node.destroy_node()
        rclpy.shutdown()