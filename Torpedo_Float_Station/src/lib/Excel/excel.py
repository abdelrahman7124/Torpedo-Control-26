import csv
import matplotlib.pyplot as plt

x = []
y = []

def plot_excel():
    with open('data.csv', mode = 'r') as file:
        reader = csv.reader(file)
        next(reader)
        for row in reader:
            print(row)
            x.append(row[0])
            y.append(row[1])
    
    plt.plot(x,y)
    plt.xlabel("Time")
    plt.ylabel("depth")
    plt.title("Depth/Time Plot")
    plt.show()

if __name__ == "main":
    plot_excel()