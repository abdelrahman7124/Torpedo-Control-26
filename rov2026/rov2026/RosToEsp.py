#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from std_msgs.msg import String, Float32MultiArray
import socket

ESP_IP = "192.168.1.44"
ESP_PORT = 9000


class RosToEsp(Node):

    def __init__(self):

        super().__init__('ros_to_esp_node')

        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

        self.last_cmd_time = self.get_clock().now()

        self.flags = [0.0,0.0,0.0,0.0,0.0,0.0]

        self.esp_addr = (ESP_IP, ESP_PORT)

        self.pwm_cmd = "1500,1500,1500,1500,1500,1500,90,0,0,0"

        self.current_cmd = self.pwm_cmd

        self.subscription = self.create_subscription(
            String,
            'esp_commands',
            self.command_callback,
            10
        )

        self.subscription2 = self.create_subscription(
            Float32MultiArray,
            'rov_flags',
            self.flags_handle,
            10
        )

        self.timer = self.create_timer(0.1, self.send_udp_packet)

        self.get_logger().info(
            f"Sender Started. Targeting {ESP_IP}:{ESP_PORT}"
        )

    def command_callback(self, msg):

        self.pwm_cmd = msg.data

        self.last_cmd_time = self.get_clock().now()

    def flags_handle(self, msg):

        self.flags = msg.data

    def send_udp_packet(self):

        try:

            elapsed = (
                self.get_clock().now() - self.last_cmd_time
            ).nanoseconds / 1e9

            if elapsed > 0.5:

                self.pwm_cmd = (
                    "1500,1500,1500,1500,1500,1500,90,0,0,0"
                )

                self.flags = [0.0,0.0,0.0,0.0,0.0,0.0]

            flags_string = ",".join(map(str, self.flags))

            self.current_cmd = self.pwm_cmd + "," + flags_string

            self.sock.sendto(
                self.current_cmd.encode(),
                self.esp_addr
            )

            self.get_logger().info(
                f"current command = {self.current_cmd}"
            )

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


if __name__ == '__main__':

    main()