import socket
import time
import math

UDP_IP = "127.0.0.1"
UDP_PORT = 8888

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

t = 0.0
while True:
    roll  = 5.0 * math.sin(t)
    pitch = 3.0 * math.sin(t * 0.5)
    yaw   = (t * 10.0) % 360.0       # slowly rotating
    depth = 1.5 + 0.5 * math.sin(t * 0.3)

    message = f"{roll:.2f},{pitch:.2f},{yaw:.2f},{depth:.2f}"
    sock.sendto(message.encode('utf-8'), (UDP_IP, UDP_PORT))
    print(f"Sent: {message}")

    t += 0.1
    time.sleep(0.5)