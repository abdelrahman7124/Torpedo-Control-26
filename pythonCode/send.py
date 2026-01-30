import socket
import time
ESP_IP = "192.168.1.55"
ESP_PORT = 9000
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

print(f"SENDER STARTED.")
print(f"Sending commands to {ESP_IP}:{ESP_PORT}...")

try:
    while True:
        command = "255,255,255,0" 
        sock.sendto(command.encode(), (ESP_IP, ESP_PORT))
        time.sleep(0.1)

except KeyboardInterrupt:
    print("\nSender Stopped.")
    sock.close()