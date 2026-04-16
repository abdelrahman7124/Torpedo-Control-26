from utils import constants,states
from lib.reciever import reciever
from lib.plot import plot
import time

depth_axis = []

def main():
    wifi = reciever.WifiReciever()
    plotting = plot.Plot()

    while(wifi.wait_for_wifi() != states.WiFiState.FOUND):
        time.sleep(constants.RESET_DURATION)

    while(wifi.init_connection() != states.WiFiState.FOUND):
        time.sleep(constants.RESET_DURATION)
    
    while(True):
        state,data = wifi.recieve()

        if (state == states.WiFiState.WAITING):
            break
        
        if (data != None):
            split_data = data.split(",")
            for i in (range(len(split_data)//constants.PACKET_SIZE)):
                depth_axis.append(float(split_data[(i * constants.PACKET_SIZE) + constants.DATA_INDEX]))
    
    plotting.plot(depth_axis)

if(__name__ == "__main__"):
    main()

    