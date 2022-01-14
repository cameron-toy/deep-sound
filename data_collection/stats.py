import sys

data = dict()
with open(sys.argv[1], "r") as f:
    for line in f.read().splitlines():
        num = int(line)
        data[num] = data.get(num, 0) + 1

print(data)
