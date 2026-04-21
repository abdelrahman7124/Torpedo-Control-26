import socket
import time
import random

UDP_IP = "127.0.0.1"
UDP_PORT = 8888

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

while True:
    roll  = random.uniform(-180.0, 180.0)
    pitch = random.uniform(-90.0, 90.0)
    yaw   = random.uniform(0.0, 360.0)
    depth = random.uniform(0.0, 10.0)

    message = f"{roll:.2f},{pitch:.2f},{yaw:.2f},{depth:.2f}"
    sock.sendto(message.encode('utf-8'), (UDP_IP, UDP_PORT))
    print(f"Sent: {message}")

    time.sleep(0.05)