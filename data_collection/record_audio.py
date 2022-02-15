from multiprocessing.sharedctypes import Value
import wave
import serial
import sys
import itertools

def blockwise(t, size=2, fillvalue=None):
    it = iter(t)
    return itertools.zip_longest(*[it]*size, fillvalue=fillvalue)

def read_serial(port, baud):
    with serial.Serial(port, baud, timeout=None) as ser:
        ser.flush()
        # Don't start reading until start sequence recieved
        print("waiting for start")
        ser.read_until(b"\xFF" * 8)
        print("waiting for stop")
        bytes = ser.read_until(b"\xFF" * 8)
        print("stop recieved")
    
    shifted_bytes = []
    for bytepair in blockwise(bytes, 2):
        value = int.from_bytes(bytepair, "little", signed=False)
        value <<= 2
        shifted_bytes.append(int.to_bytes(value, "little", signed=False))

    
    return b"".join(shifted_bytes)


def record(framerate, fname, data):
    with wave.open(fname, "w") as f:
        f.setnchannels(1)
        f.setsampwidth(2)
        f.setframerate(framerate)
        f.writeframes(data)

def main():
    try:
        _, port, baud, framerate, fname = sys.argv
    except ValueError:
        print("usage: python record_audio.py PORT BAUD SAMPLE_RATE FILENAME")
        sys.exit(1)

    data = read_serial(port, baud)
    record(framerate, fname, data)

if __name__=="__main__":
    main()