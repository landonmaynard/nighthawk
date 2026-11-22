from lib.link import Link
from lib.packet import Packet, HeartbeatSample, GyroSample
import time
link = Link()
packet = Packet()


port = "/dev/ttyUSB0"
baudrate = 14400

link.initialize(port, baudrate)

while True:
    recieved = link.read()
    newSample = packet.parse(recieved)
    if newSample is not None:
        print("New Sample")
        print(newSample)
        print(time.monotonic())
    match newSample:
        case None:
            pass
        case HeartbeatSample():
            heartbeatSample = HeartbeatSample(timestamp=newSample.timestamp)
        case GyroSample():
            gyroSample = GyroSample(timestamp = newSample.timestamp, x = newSample.x, y = newSample.y, z=newSample.z)
    