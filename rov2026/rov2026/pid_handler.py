#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from std_msgs.msg import String
import json


class PIDReceiver(Node):

    def __init__(self):

        super().__init__('pid_receiver')

        self.subscription = self.create_subscription(
            String,
            'pid_val',
            self.pid_callback,
            10
        )

        self.pid_data = {}

        self.get_logger().info("PID Receiver Started")

    def pid_callback(self, msg):

        try:

            self.pid_data = json.loads(msg.data)

            kp = self.pid_data["Kp"]
            ki = self.pid_data["Ki"]
            kd = self.pid_data["Kd"]

            self.get_logger().info(
                f"Kp={kp}, Ki={ki}, Kd={kd}"
            )

        except Exception as e:

            self.get_logger().error(
                f"Failed to parse PID JSON: {e}"
            )


def main(args=None):

    rclpy.init(args=args)

    node = PIDReceiver()

    try:
        rclpy.spin(node)

    except KeyboardInterrupt:
        pass

    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()