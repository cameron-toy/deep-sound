import serial
import sys
import itertools
import plotting

SAMPLE_RATE = 15000
FFT_SIZE = 1024

def blockwise(t, size=2, fillvalue=None):
    it = iter(t)
    return itertools.zip_longest(*[it]*size, fillvalue=fillvalue)

def main():
    _, port, baud, fname = sys.argv
    # COM4 = /dev/ttyS4
    with serial.Serial(port, baud, timeout=None) as ser:
        ser.flush()
        # Don't start reading until start sequence recieved
        print("waiting for start")
        garb = ser.read_until(b"\xFF" * 8)
        print(str(garb))
        print("waiting for stop")
        bytes = ser.read_until(b"\xFF" * 8)
        print("stop recieved")

    values = []
    freq_intervals = [0]
    for bytepair in blockwise(bytes, 2):
        value = int.from_bytes(bytepair, "little", signed=False)
        value = (value & 0x7FFF) - (value & 0x8000) # Make signed
        values.append(value)
        freq_intervals.append(freq_intervals[-1] + SAMPLE_RATE / FFT_SIZE)


    with open(fname, "w") as f:
        f.write("\n".join([str(val) for val in values]))

    # Plot up to nyquist frequency
    plotting.plot_data(freq_intervals[:len(freq_intervals) >> 1],
                       values[:len(values) >> 1])

if __name__=="__main__":
    main()
