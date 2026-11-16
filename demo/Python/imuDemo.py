import numpy as np
import matplotlib.pyplot as plt
import serial
import time
from scipy.spatial.transform import Rotation
from crccheck.crc import Crc8Smbus
import struct
import pyvista as pv
ser = serial.Serial("/dev/ttyUSB0", 19200, timeout=0)
# /home/lando/Documents/Nighthawk/Python/imuDemoCode/.venv/bin/python -m serial.tools.list_ports

#GLOBAL CONFIG
debugLevel = 1 # 0 off, 1 simple, 2 advanced
packetLength = 18
gxBias = 0.338
gyBias = -0.812
gzBias = -0.261

#SETUP
receiveBuffer = bytearray()
packet = None
timestamp = None
gx = None
gy = None
gz = None
ax = None
ay = None
az = None
packetCounter = None
telemetryFlags = None
acceptedPackets = 0
rejectedPackets = 0
lastSimpleTime = time.perf_counter()
lastAdvancedTime = time.perf_counter()
lastPacketCounter = None
missedPackets = 0
lastTimestamp = None
lastReceivedTime = None
elapsedTime = 0
connected = False

#PYVISTA SETUP
model = pv.read('/home/lando/Documents/Nighthawk/Python/imuDemoCode/demoRocket.stl')
model.translate(-np.array(model.center), inplace=True)
viewer = pv.Plotter()
viewer.set_background("black")
rocket = viewer.add_mesh(model, color="white")
rocket.origin = model.center
rocket.orientation = (0, 0, 0)
viewer.add_axes()
viewer.show(interactive_update=True, auto_close=False)
def resetOrientation():
    global orientation
    orientation = Rotation.identity()
    rocket.user_matrix = np.eye(4)
viewer.add_key_event("r", resetOrientation)
orientation = Rotation.identity()
modelCounter = 0
modelRatio = 5

#MATPLOTLIB SETUP
windowLength = 1000
sampleCount = 0
plt.ion()
plt.style.use("dark_background")
fig, (accelGraph, gyroGraph) = plt.subplots(2, 1)
axLine, = accelGraph.plot([], [])
ayLine, = accelGraph.plot([], [])
azLine, = accelGraph.plot([], [])
gxLine, = gyroGraph.plot([], [])
gyLine, = gyroGraph.plot([], [])
gzLine, = gyroGraph.plot([], [])
accelGraph.set_ylim(-8, 8)
gyroGraph.set_ylim(-500, 500)
accelGraph.set_xlim(0, windowLength)
gyroGraph.set_xlim(0, windowLength)
graphRatio = 5
graphCounter = 0
sampleValues = []
axValues = []
ayValues = []
azValues = []
gxValues = []
gyValues = []
gzValues = []

#FUNCTIONS
def printDebug(message, level):
    if debugLevel >= level:
        print(message)

def simpleSummary():
    global acceptedPackets, rejectedPackets, missedPackets, lastSimpleTime
    if time.perf_counter() - lastSimpleTime >= 1:
        if debugLevel >= 1:
            print("Simple Summary")
            print("Flags - ", format(telemetryFlags, "08b"))
            print("Accepted Packets - ", acceptedPackets)
            print("Rejected Packets - ", rejectedPackets)
            print("Missed Packets - ", missedPackets)
            print("Buffer Length - ", len(receiveBuffer))
            print()
            acceptedPackets = 0
            rejectedPackets = 0
            missedPackets = 0
            lastSimpleTime = time.perf_counter()

def advancedSummary():
    global lastAdvancedTime
    if time.perf_counter() - lastAdvancedTime >= 0.5:
        if debugLevel >= 2:
            print("Advanced Summary")
            print("Timestamp - ", timestamp)
            print("gX - ", gx)
            print("gY - ", gy)
            print("gZ - ", gz)
            print("aX - ", ax)
            print("aY - ", ay)
            print("aZ - ", az)
            print("Packet Counter - ", packetCounter)
            print()
            lastAdvancedTime = time.perf_counter()


def receiveBytes():
    incoming = ser.read(ser.in_waiting)
    receiveBuffer.extend(incoming)

def parsePacket():
    global acceptedPackets, rejectedPackets, lastReceivedTime
    if len(receiveBuffer) < 18:
        return None
    while len(receiveBuffer) >= 2 and (receiveBuffer[0] != 0xAA or receiveBuffer[1] != 0x55):
        del receiveBuffer[0]
    if len(receiveBuffer) >= 18 and receiveBuffer[0] == 0xAA and receiveBuffer [1] == 0x55:
        candidate = bytes(receiveBuffer[:18])
        calculatedCRC = Crc8Smbus.calc(candidate[2:17])
        if candidate[17] == calculatedCRC:
            del receiveBuffer[:18]
            acceptedPackets += 1
            lastReceivedTime = time.perf_counter()
            return candidate
        else:
            del receiveBuffer[0]
            rejectedPackets += 1
    return None

def decodePacket(rawPacket):
    decodedTimestamp = ((struct.unpack_from("<I", rawPacket, 2)[0]))
    decodedGX = np.interp((struct.unpack_from("<h", rawPacket, 6)[0]), [-32767, 32767], [-500, 500])
    decodedGY = np.interp((struct.unpack_from("<h", rawPacket, 8)[0]), [-32767, 32767], [-500, 500])
    decodedGZ = np.interp((struct.unpack_from("<h", rawPacket, 10)[0]), [-32767, 32767], [-500, 500])
    decodedAX = np.interp((struct.unpack_from("<b", rawPacket, 12)[0]), [-127, 127], [-8, 8])
    decodedAY = np.interp((struct.unpack_from("<b", rawPacket, 13)[0]), [-127, 127], [-8, 8])
    decodedAZ = np.interp((struct.unpack_from("<b", rawPacket, 14)[0]), [-127, 127], [-8, 8])
    decodedPacketCounter = (struct.unpack_from("<B", rawPacket, 15)[0])
    decodedTelemetryFlags = (struct.unpack_from("<B", rawPacket, 16)[0])
    return decodedTimestamp, decodedGX, decodedGY, decodedGZ, decodedAX, decodedAY, decodedAZ, decodedPacketCounter, decodedTelemetryFlags

def checkMissedPackets(counter):
    global lastPacketCounter, missedPackets
    if lastPacketCounter is not None:
        difference = (counter - lastPacketCounter) % 256
        if difference > 1:
            missedPackets += difference - 1
    lastPacketCounter = counter
    return missedPackets

def checkElapsedTime(time):
    global elapsedTime, lastTimestamp
    if lastTimestamp is not None:
        elapsedTime = ((time - lastTimestamp) % (2**32)) / 1000000
    lastTimestamp = time
    return elapsedTime

def checkConnection():
    global connected
    connected = (lastReceivedTime is not None and time.perf_counter() - lastReceivedTime < 1)

def getOrientation():
    global orientation
    x = gx - gxBias
    y = gy - gyBias
    z = gz - gzBias
    rotationStep = np.deg2rad([z, y, -x]) * elapsedTime
    change = Rotation.from_rotvec(rotationStep)
    orientation = orientation * change

def appendValues():
    global sampleCount, sampleValues, gx, gxValues, gy, gyValues, gz, gzValues, ax, axValues, ay, ayValues, az, azValues, modelCounter, graphCounter
    sampleCount += 1

    sampleValues.append(sampleCount)
    if len(sampleValues) > windowLength:
        sampleValues.pop(0)
    gxValues.append(gx)
    if len(gxValues) > windowLength:
        gxValues.pop(0)
    gyValues.append(gy)
    if len(gyValues) > windowLength:
        gyValues.pop(0)
    gzValues.append(gz)
    if len(gzValues) > windowLength:
        gzValues.pop(0)
    axValues.append(ax)
    if len(axValues) > windowLength:
        axValues.pop(0)
    ayValues.append(ay)
    if len(ayValues) > windowLength:
        ayValues.pop(0)
    azValues.append(az)
    if len(azValues) > windowLength:
        azValues.pop(0)

    modelCounter += 1
    graphCounter += 1

def graphValues():
    global graphCounter
    if graphCounter >= graphRatio:
        graphCounter = 0
        axLine.set_data(sampleValues, axValues)
        ayLine.set_data(sampleValues, ayValues)
        azLine.set_data(sampleValues, azValues)
        gxLine.set_data(sampleValues, gxValues)
        gyLine.set_data(sampleValues, gyValues)
        gzLine.set_data(sampleValues, gzValues)

        accelGraph.set_xlim((sampleCount-windowLength), sampleCount)
        gyroGraph.set_xlim((sampleCount-windowLength), sampleCount)
        plt.pause(0.001)

def modelValues():
    global modelCounter

    if modelCounter >= modelRatio:
        modelCounter = 0

        transform = np.eye(4)
        transform[:3, :3] = orientation.as_matrix()

        rocket.user_matrix = transform
        viewer.update()
    
try:
    while True:
        receiveBytes()
        packet = parsePacket()
        if packet is not None:
            timestamp, gx, gy, gz, ax, ay, az, packetCounter, telemetryFlags = decodePacket(packet)
            checkElapsedTime(timestamp)
            checkMissedPackets(packetCounter)
            getOrientation()
            appendValues()
        checkConnection()
        graphValues()
        modelValues()
        if timestamp is not None:
            simpleSummary()
            advancedSummary() 

except KeyboardInterrupt:
    print("Stopped")

finally:
    ser.close()
    plt.close("all")
    viewer.close()
        

    
