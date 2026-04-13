import socket
import time
import subprocess

buffer = ""
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)


TARGET_SSID = "Topedo_Float"
ESP32_IP = "192.168.4.1"
PORT = 1234



def get_ssid():
    try:
        result = subprocess.check_output(
            ["nmcli", "-t", "-f", "active,ssid", "dev", "wifi"],
            text=True
        )

        for line in result.splitlines():
            active, ssid = line.split(":")
            if active == "yes":
                return ssid

    except Exception:
        return None

    return None


def wait_for_wifi():
    while True:
        ssid = get_ssid()

        if ssid == TARGET_SSID:
            print("Correct WiFi detected:", ssid)
            return

        print("Wrong network:", ssid, "→ waiting for ",TARGET_SSID)
        time.sleep(2)

def init_connection():
    while (True):
        print("Trying to connect...")
        sock.connect(("192.168.4.1", 1234))
        print("Connected!")
        break
            
        

        
def recieve(buffer):
    while (True):
        data = sock.recv(1024)
        if(not data):
            print("Data not recieved!")
            init_connection()

        else:
            buffer += data.decode()
            if "\n" in buffer:
                lines = buffer.split("\n")
                for line in lines:
                 print(line)

if __name__ == "__main__":
    wait_for_wifi()
    init_connection()
    recieve(buffer)



    