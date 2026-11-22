from dataclasses import dataclass
from crccheck.crc import Crc8Smbus
from lib import globals
import time

#data classes for each type of packet
@dataclass
class GyroSample:
    timestamp: int
    x: int
    y: int
    z: int

@dataclass
class HeartbeatSample:
    timestamp: float

class Packet:
    def __init__(self):
        pass
    def parse(self, buffer):
        if len(buffer) >= 3:
            if buffer[0] == globals.startByte1 and buffer[1] == globals.startByte2:
                packetType = buffer[2]
                match packetType:
                    case globals.gyroPacketType: #gyro packet
                        if len(buffer) >= globals.gyroPacketLength:
                            candidate = self.decodeGyro(buffer[:globals.gyroPacketLength])
                            del buffer[:globals.gyroPacketLength]
                            return candidate
                    case globals.heartbeatPacketType:
                        if len(buffer) >= globals.heartbeatPacketLength:
                            candidate = self.decodeHeartbeat()
                            del buffer[:globals.heartbeatPacketLength]
                            return candidate
                    case _: #default
                        buffer.pop(0)
                        return None
            else:
                buffer.pop(0)
        else:
            return None

    def decodeHeartbeat(self):
        sample = HeartbeatSample(timestamp=time.monotonic())
        return sample

    def decodeGyro(self, packetCandidate):
        calculatedCRC = Crc8Smbus.calc(packetCandidate[2:14])
        if calculatedCRC == packetCandidate[14]:
            timestamp = int.from_bytes(packetCandidate[3:7], 'big', signed=False)
            x = int.from_bytes(packetCandidate[7:9], 'big', signed=True)
            y = int.from_bytes(packetCandidate[9:11], 'big', signed=True)
            z = int.from_bytes(packetCandidate[11:13], 'big', signed=True)
            sample = GyroSample(timestamp=timestamp, x=x, y=y, z=z)
            return sample
        else:
            return None
    