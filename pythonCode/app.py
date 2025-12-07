import socket

ESP_IP = "192.168.1.30"
ESP_PORT = 9000

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(('0.0.0.0', 9000))
sock.settimeout(2)
while True:
    try:
        data, addr = sock.recvfrom(1024)
        print(data.decode())
    except socket.timeout:
        print("No reply received. Check connections.")