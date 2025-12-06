import rclpy
from rclpy.node import Node
import socket
class imu_ethernet_node(Node):
    def __init__(self):
        super().__init__("ethernet_node")
        self.ESP_IP = "192.168.1.20"
        self.ESP_PORT = 8888
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind(('0.0.0.0', 8888))
        self.sock.settimeout(2)
        # self.create_timer(0.5,self.recieveData)
        while True:
            self.recieveData()
    def recieveData(self):
        try:
            data, addr = self.sock.recvfrom(1024)
            print(data.decode())
        except socket.timeout:
            print("No reply received. Check connections.")
def main(args=None):
    rclpy.init(args=args)
    node = imu_ethernet_node()
    rclpy.spin(node)
    rclpy.shutdown()

if __name__ == '__main__':
    main()