import argparse
import sys
import time

import numpy as np
import sounddevice as sd


def int_or_str(text):
    """Helper function for argument parsing."""
    try:
        return int(text)
    except ValueError:
        return text


parser = argparse.ArgumentParser(add_help=False)
parser.add_argument(
    '-l', '--list-devices', action='store_true',
    help='show list of audio devices and exit')
args, remaining = parser.parse_known_args()
if args.list_devices:
    print(sd.query_devices())
    parser.exit(0)
parser = argparse.ArgumentParser(
    description=__doc__,
    formatter_class=argparse.RawDescriptionHelpFormatter,
    parents=[parser])
parser.add_argument(
    'frequency1', nargs='?', metavar='FREQUENCY1', type=float, default=500,
    help='first frequency in Hz (default: %(default)s)')
parser.add_argument(
    'frequency2', nargs='?', metavar='FREQUENCY2', type=float, default=800,
    help='second frequency in Hz (default: %(default)s)')
parser.add_argument(
    'time1', nargs='?', metavar='TIME1', type=float, default=1,
    help='duration of first frequency in seconds (default: %(default)s)')
parser.add_argument(
    'time2', nargs='?', metavar='TIME2', type=float, default=1,
    help='duration of second frequency in seconds (default: %(default)s)')
parser.add_argument(
    'reps', nargs='?', metavar="REPS", type=int, default=8,
    help="number of repetitions to perform chrips. zero is infinite. (default: %(default)s")
parser.add_argument(
    '-d', '--device', type=int_or_str,
    help='output device (numeric ID or substring)')
parser.add_argument(
    '-a', '--amplitude', type=float, default=0.2,
    help='amplitude (default: %(default)s)')
main_args = parser.parse_args(remaining)

start_idx = 0
time_since = 0
fcurr = 0
transitions = 0

print(args)

def do_knock(args):
    try:
        samplerate = sd.query_devices(args.device, 'output')['default_samplerate']

        def callback(outdata, frames, time, status):
            if status:
                print(status, file=sys.stderr)

            global start_idx
            global fcurr
            global time_since
            global transitions

            t = (start_idx + np.arange(frames)) / samplerate
            t = t.reshape(-1, 1)
            if fcurr == 0:
                outdata[:] = args.amplitude * np.sin(2 * np.pi * args.frequency1 * t)
                if time.currentTime - time_since >= args.time1:
                    time_since = time.currentTime
                    transitions += 1
                    fcurr = 1
            else:
                outdata[:] = args.amplitude * np.sin(2 * np.pi * args.frequency2 * t)
                if time.currentTime - time_since >= args.time2:
                    time_since = time.currentTime
                    transitions += 1
                    fcurr = 0

            start_idx += frames

        with sd.OutputStream(device=args.device, channels=1, callback=callback,
                            samplerate=samplerate):
            while transitions == 0 or transitions < args.reps:
                time.sleep(2)

    except KeyboardInterrupt:
        parser.exit('')
    except Exception as e:
        parser.exit(type(e).__name__ + ': ' + str(e))


if __name__=="__main__":
    do_knock(main_args)
