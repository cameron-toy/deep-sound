import os
import matplotlib.pyplot as plt

def plot_data(x, height):
    plt.bar(x, height, width=30, label="Frequency Magnitude")
    plt.legend()
    plt.xlabel('Frequency')
    plt.ylabel('Magnitude')
    plt.title('Frequency Spectrum')
    # can't display plot in WSL
    if os.name == 'nt':
        plt.show()
    plt.savefig("spectrum.png")
