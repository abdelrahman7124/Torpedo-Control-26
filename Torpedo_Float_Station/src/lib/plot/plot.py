from utils import constants
import matplotlib.pyplot as plt

class Plot():
    def __init__(self):
        self.sample_duration = constants.SAMPLE_DURATION
        self.depth_axis = []
        self.time_axis = []
    
    def plot(self,depth,time):
        self.depth_axis = depth
        self.time_axis = time

        plt.plot(self.time_axis,self.depth_axis)
        
        plt.xlabel("Time(ms)")
        plt.ylabel("Depth(m)")
        plt.title("Depth/Time Plot")
        
        plt.grid(True)
        plt.show()