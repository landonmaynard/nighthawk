import serial
byte = 0
class Link:
    def initialize():
        ser = serial.Serial("/dev/ttyUSB0", 19200)
    def read():
        byte = ser.read(1)
        print(byte)