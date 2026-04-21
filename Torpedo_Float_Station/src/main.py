from utils import constants, states
from lib.reciever import reciever
from lib.plot import plot
import time

depth_axis = []
time_axis = []

def main():
    wifi = reciever.WifiReciever()
    plotting = plot.Plot()

    while wifi.wait_for_wifi() != states.WiFiState.FOUND:
        time.sleep(constants.RESET_DURATION)

    while wifi.init_connection() != states.WiFiState.FOUND:
        time.sleep(constants.RESET_DURATION)

    wifi.send()
    time.sleep(0.5)
    wifi.close()

    while wifi.init_connection() != states.WiFiState.FOUND:
        time.sleep(constants.RESET_DURATION)

    while True:
        state, data = wifi.recieve()

        if state == states.WiFiState.DISCONNECTED:
            print("ESP disconnected. Plotting...")
            break 

        if state in (states.WiFiState.WAITING, states.WiFiState.RECEIVING):
            continue

        if state == states.WiFiState.FOUND:
            for line in data:
                parts = line.strip().split(",")
                if len(parts) >= constants.PACKET_SIZE:
                    try:
                        depth = float(parts[constants.DEPTH_INDEX])
                        time = parts[constants.TIME_INDEX]

                        depth_axis.append(depth)
                        time_axis.append(time)
                    
                    except ValueError:
                        pass
    plotting.plot(depth_axis, time_axis)

if __name__ == "__main__":
    main()