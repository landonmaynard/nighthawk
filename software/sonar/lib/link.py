import serial

class Link:
    def __init__(self):
        pass

    def initialize(port, baudrate):
        self.ser = serial.Serial(port, baudrate, timeout=1)