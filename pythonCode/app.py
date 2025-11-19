import socket

ESP_IP = "192.168.1.177"
ESP_PORT = 8888

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.settimeout(2)

try:
    message = input("Enter the message= ")
    sock.sendto(message.encode(), (ESP_IP, ESP_PORT))
    print(f"Sent: {message}")

    data, addr = sock.recvfrom(1024)
    print(f"Received: {data.decode()} from {addr}")
except socket.timeout:
    print("No reply received. Check connections.")