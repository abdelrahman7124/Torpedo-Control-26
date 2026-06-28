#!/usr/bin/env python3

import rclpy

from rclpy.node import Node
from sensor_msgs.msg import Joy
from std_msgs.msg import Float32MultiArray


class EspToRos(Node):

    def __init__(self):

        super().__init__('flags')

        self.pub = self.create_publisher(Float32MultiArray, 'rov_flags', 10)

        self.sub = self.create_subscription(Joy,'joy',self.joy_callback,10)

    def joy_callback(self, msg):

        data = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0]

        # Left analog up/down
        if msg.axes[1] > 0:
            data[0] = msg.axes[1]      # forward

        if msg.axes[1] < 0:
            data[1] = abs(msg.axes[1]) # backward

        # Left analog left/right
        if msg.axes[0] < 0:
            data[2] = abs(msg.axes[0]) # left heave

        if msg.axes[0] > 0:
            data[3] = msg.axes[0]      # right heave

        # Right analog left/right
        if msg.axes[3] < 0:
            data[4] = abs(msg.axes[3]) # left yaw

        if msg.axes[3] > 0:
            data[5] = msg.axes[3]      # right yaw

        out = Float32MultiArray()

        out.data = data

        self.pub.publish(out)

        print(data)


def main(args=None):

    rclpy.init(args=args)

    node = EspToRos()

    rclpy.spin(node)

    node.destroy_node()

    rclpy.shutdown()


if __name__ == '__main__':
    main()