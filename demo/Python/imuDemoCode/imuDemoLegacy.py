import numpy as np
import matplotlib.pyplot as plt
import serial
import time
ser = serial.Serial("/dev/ttyUSB0", 19200)

# python -m serial.tools.list_ports

data = [127, 127, 127, 127]
azRaw = None
gxRaw = None
gyRaw = None
gzRaw = None
azConverted = None
gxConverted = None
gyConverted = None
gzConverted = None

import pyvista as pv
model = pv.read('/home/lando/Documents/Nighthawk/Python/imuDemoCode/demoRocket.stl')
viewer = pv.Plotter()
viewer.set_background("black")
rocket = viewer.add_mesh(model, color="white")
rocket.origin = model.center
rocket.orientation = (0, 0, 0)
viewer.add_axes()
viewer.show(interactive_update=True, auto_close=False)

lastSampleTime = time.perf_counter()
currentTime = lastSampleTime
elapsedTime = None

x = y = z = 0.0

threeDCounter = 0
threeDRatio = 5

accelScale = 8
gyroScale = 500

accelRawScale = accelScale * 1000
gyroRawScale = gyroScale * 1000

azConvertedPrevious = 0.0;
gxConvertedPrevious = 0.0;
gyConvertedPrevious = 0.0;
gzConvertedPrevious = 0.0;

def getOrientation():
    global x, y, z, lastSampleTime, currentTime, elapsedTime
    currentTime = time.perf_counter()
    elapsedTime = currentTime - lastSampleTime
    lastSampleTime = currentTime
    x += gxConverted * elapsedTime
    y += gyConverted * elapsedTime
    z += gzConverted * elapsedTime

def readData():
    global data
    byte = ser.read(1)
    if byte[0] == 255:
        data = list(ser.read(4))
        return True
    return False

cutThreshold = 5
    
def rawData():
    global azRaw, gxRaw, gyRaw, gzRaw

    if abs(data[0] - 127) <= cutThreshold:
        data[0] = 127
    if abs(data[1] - 127) <= cutThreshold:
        data[1] = 127
    if abs(data[2] - 127) <= cutThreshold:
        data[2] = 127
    if abs(data[3] - 127) <= cutThreshold:
        data[3] = 127

    

    azRaw = np.interp(data[0], [0, 254], [-(accelRawScale),accelRawScale])

    gxRaw = np.interp(data[1], [0, 254], [-(gyroRawScale),gyroRawScale])

    gyRaw = np.interp(data[2], [0, 254], [-(gyroRawScale),gyroRawScale])

    gzRaw = np.interp(data[3], [0, 254], [-(gyroRawScale),gyroRawScale])

def convertData():
    global azConverted, gxConverted, gyConverted, gzConverted
    global azConvertedPrevious, gxConvertedPrevious, gyConvertedPrevious, gzConvertedPrevious
    rawData()
    azConverted = azRaw/1000
    gxConverted = gxRaw/1000
    gyConverted = gyRaw/1000
    gzConverted = gzRaw/1000

    if abs(azConverted-azConvertedPrevious) >= (accelScale/1.25):
        azConverted = azConvertedPrevious
    if abs(gxConverted-gxConvertedPrevious) >= (gyroScale/1.25):
        gxConverted = gxConvertedPrevious
    if abs(gyConverted-gyConvertedPrevious) >= (gyroScale/1.25):
        gyConverted = gyConvertedPrevious
    if abs(gzConverted-gzConvertedPrevious) >= (gyroScale/1.25):
        gzConverted = gzConvertedPrevious

    azConvertedPrevious = azConverted
    gxConvertedPrevious = gxConverted
    gyConvertedPrevious = gyConverted
    gzConvertedPrevious = gzConverted

windowLength = 1000
sampleCount = 0
graphCounter = 0;
graphRatio = 5;

plt.style.use("dark_background")
fig, (accelGraph, gyroGraph) = plt.subplots(2, 1)
azLine, = accelGraph.plot([], [])
gxLine, = gyroGraph.plot([], [])
gyLine, = gyroGraph.plot([], [])
gzLine, = gyroGraph.plot([], [])
accelGraph.set_ylim(-(accelScale), accelScale)
gyroGraph.set_ylim(-(gyroScale), gyroScale)
accelGraph.set_xlim(0, windowLength)
gyroGraph.set_xlim(0, windowLength)

def resetOrientation():
    global x, y, z, lastSampleTime

    x = y = z = 0.0
    lastSampleTime = time.perf_counter()
    rocket.orientation = (0, 0, 0)

viewer.add_key_event("r", resetOrientation)

sampleValues = []
azValues = []
gxValues = []
gyValues = []
gzValues = []

sensitivity = 1.8

while True:
    if readData():
        convertData()
        getOrientation()

        sampleCount += 1
        sampleValues.append(sampleCount)
        if len(sampleValues) > windowLength:
            sampleValues.pop(0)

        azValues.append(azConverted)
        if len(azValues) > windowLength:
            azValues.pop(0)

        gxValues.append(gxConverted)
        if len(gxValues) > windowLength:
            gxValues.pop(0)

        gyValues.append(gyConverted)
        if len(gyValues) > windowLength:
            gyValues.pop(0)

        gzValues.append(gzConverted)
        if len(gzValues) > windowLength:
            gzValues.pop(0)

        threeDCounter += 1
        graphCounter += 1

    if graphCounter >= graphRatio:
        graphCounter = 0

        azLine.set_data(sampleValues, azValues)
        gxLine.set_data(sampleValues, gxValues)
        gyLine.set_data(sampleValues, gyValues)
        gzLine.set_data(sampleValues, gzValues)

        accelGraph.set_xlim((sampleCount-windowLength), sampleCount)
        gyroGraph.set_xlim((sampleCount-windowLength), sampleCount)

        plt.pause(0.001)

    if threeDCounter >= threeDRatio:
        threeDCounter = 0
        rocket.orientation = (z * sensitivity, y * sensitivity, -x * sensitivity)
        viewer.update()
