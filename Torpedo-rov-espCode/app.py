import socket

# --- Configuration ---
UDP_IP = "0.0.0.0" 
UDP_PORT = 8888    # Make sure this matches your ESP32 port

# Create and bind the UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

print(f"Listening for raw UDP data on port {UDP_PORT}...\n")

try:
    while True:
        # Receive data
        data, addr = sock.recvfrom(1024)
        
        # Just print the raw data and where it came from
        print(f"From {addr[0]}: {data}")

except KeyboardInterrupt:
    print("\nStopped.")
finally:
    sock.close()
