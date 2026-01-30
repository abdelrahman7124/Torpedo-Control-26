import socket

LISTEN_IP = "0.0.0.0"
LISTEN_PORT = 8888

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((LISTEN_IP, LISTEN_PORT))

print(f"RECEIVER STARTED.")
print(f"Listening for data on Port {LISTEN_PORT}...")

try:
    while True:
        data, addr = sock.recvfrom(1024)
        sensor_data = data.decode('utf-8').strip()
        print(f"Received from ESP32: {sensor_data}")

except KeyboardInterrupt:
    print("\nReceiver Stopped.")
    sock.close()