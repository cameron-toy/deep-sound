from multiprocessing.sharedctypes import Value
import wave
import serial
import sys

def read_serial(port, baud):
    with serial.Serial(port, baud, timeout=None) as ser:
        ser.flush()
        # Don't start reading until start sequence recieved
        print("waiting for start")
        ser.read_until(b"\xFF" * 8)
        print("waiting for stop")
        bytes = ser.read_until(b"\xFF" * 8)
        print("stop recieved")
    
    return bytes


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