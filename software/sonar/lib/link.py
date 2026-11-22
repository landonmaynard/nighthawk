import serial

class Link:
    def __init__(self):
        self.buffer = bytearray()

    def initialize(self, port, baudrate):
        self.ser = serial.Serial(port, baudrate, timeout=0)

    def read(self):
        self.buffer.extend(self.ser.read(1))
        return self.buffer