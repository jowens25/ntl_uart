import serial
import sys

port = serial.Serial("/dev/ttyUSB0", 115200)

with open(sys.argv[1]) as f:
    for line in f:
        if line.startswith("$WC"):
            line = line.strip("\n")
            line = line + "\r\n"
            print(line.encode("utf-8", "ignore"))
            print(port.write(line.encode("utf-8", "ignore")))
