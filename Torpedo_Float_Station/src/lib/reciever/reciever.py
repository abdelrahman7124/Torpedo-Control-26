from utils import constants, states
import socket
import subprocess

class WifiReciever():
    def __init__(self):
        self.buffer = ""
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.settimeout(constants.CONNECTION_TIMEOUT_DURATION)
        self.Target_SSID = constants.DEFAULT_SSID
        self.ESP_IP = constants.DEFAULT_ESP_IP
        self.port = constants.DEFAULT_PORT
        self.timeout_count = 0        
        self.msg = "Begin"
    
    def get_ssid(self):
        try:
            result = subprocess.check_output(
                ["nmcli", "-t", "-f", "active,ssid", "dev", "wifi"],
                text=True
            )

            for line in result.splitlines():
                active, ssid = line.split(":")
                if active == "yes":
                    return states.WiFiState.FOUND, ssid
    
        except Exception:
            return states.WiFiState.ERROR, None

        return states.WiFiState.WAITING, None
    
    def wait_for_wifi(self):
        state, ssid = self.get_ssid()
        if(state == states.WiFiState.FOUND):
            if ssid == self.Target_SSID:
                print("Correct WiFi detected:", ssid)
                return states.WiFiState.FOUND

            print("Wrong network:", ssid, "→ waiting for ",self.Target_SSID)
            return states.WiFiState.WAITING
        
        elif(state == states.WiFiState.ERROR):
            print("Connection Error")
            return states.WiFiState.ERROR
        
        else:
            print("Waiting for Wifi")
            return states.WiFiState.WAITING

    
    def init_connection(self):
        try:
            print("Connecting...")
            self.sock.connect((self.ESP_IP, self.port))
            print("Connected!")
            return states.WiFiState.FOUND
        
        except:        
            return states.WiFiState.WAITING
        
    def recieve(self):
        try:
            data = self.sock.recv(constants.BUFFER_SIZE)
            self.timeout_count = 0

        except socket.timeout:
            self.timeout_count += 1
            if(self.timeout_count >= constants.MAX_TIMEOUT_COUNT):
                print("Connection silent, closing.")
                self.close()
                return states.WiFiState.DISCONNECTED, None
            return states.WiFiState.WAITING, None
        
        except socket.error as e:
            print(f"Socket error: {e}")
            return states.WiFiState.DISCONNECTED, None

        if not data:
            print("Connection closed by ESP!")
            self.close()
            return states.WiFiState.DISCONNECTED, None

        self.buffer += data.decode()

        if "\n" in self.buffer:
            lines = self.buffer.split("\n")
            self.buffer = lines[-1]
            for line in lines[:-1]:
                print(line)
            return states.WiFiState.FOUND, lines[:-1]

        return states.WiFiState.RECEIVING, None
    
    def send(self):
        try:
            self.sock.sendall((self.msg + "\n").encode())
        except Exception as e:
            print("Send error:", e)
            self.close()
    
    def close(self):
        try:
            self.sock.shutdown(socket.SHUT_RDWR)
        except Exception:
            pass

        try:
            self.sock.close()
        except Exception as e:
            print("Close error:", e)

        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        self.buffer = ""

        print("Connection closed")