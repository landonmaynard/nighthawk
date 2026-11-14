import numpy as np
import matplotlib.pyplot as plt
import time

import random
temp = 1

data = [127, 127, 127, 127]
azRaw = None
gxRaw = None
gyRaw = None
gzRaw = None
azConverted = None
gxConverted = None
gyConverted = None
gzConverted = None

def readData():
    global data

    data[0] += random.randint(-temp, temp)
    data[1] += random.randint(-temp, temp)
    data[2] += random.randint(-temp, temp)
    data[3] += random.randint(-temp, temp)

    data[0] = max(0, min(data[0], 254))
    data[1] = max(0, min(data[1], 254))
    data[2] = max(0, min(data[2], 254))
    data[3] = max(0, min(data[3], 254))

    return True
    
def rawData():
    global azRaw, gxRaw, gyRaw, gzRaw
    azRaw = np.interp(data[0], [0, 254], [-16000,16000])

    gxRaw = np.interp(data[1], [0, 254], [-1000000,1000000])

    gyRaw = np.interp(data[2], [0, 254], [-1000000,1000000])

    gzRaw = np.interp(data[3], [0, 254], [-1000000,1000000])

def convertData():
    global azConverted, gxConverted, gyConverted, gzConverted
    rawData()
    azConverted = azRaw/1000
    gxConverted = gxRaw/1000
    gyConverted = gyRaw/1000
    gzConverted = gzRaw/1000

windowLength = 1000
sampleCount = 0
graphCounter = 0;
graphRatio = 5;

fig, (accelGraph, gyroGraph) = plt.subplots(2, 1)
azLine, = accelGraph.plot([], [])
gxLine, = gyroGraph.plot([], [])
gyLine, = gyroGraph.plot([], [])
gzLine, = gyroGraph.plot([], [])
accelGraph.set_ylim(-16, 16)
gyroGraph.set_ylim(-1000, 1000)
accelGraph.set_xlim(0, windowLength)
gyroGraph.set_xlim(0, windowLength)

sampleValues = []
azValues = []
gxValues = []
gyValues = []
gzValues = []

threeDCounter = 0
threeDRatio = 5

import pyvista as pv
model = pv.read('/home/lando/Documents/Nighthawk/Python/imuDemoCode/RocketV1.stl')
viewer = pv.Plotter()
rocket = viewer.add_mesh(model, color="ivory_black")
rocket.origin = model.center
rocket.orientation = (0, 0, 0)
viewer.add_axes()
viewer.show(interactive_update=True, auto_close=False)

lastSampleTime = time.perf_counter()
currentTime = lastSampleTime
elapsedTime = None

x = y = z = 0.0

def getOrientation():
    global x, y, z, lastSampleTime, currentTime, elapsedTime
    currentTime = time.perf_counter()
    elapsedTime = currentTime - lastSampleTime
    lastSampleTime = currentTime
    x += gxConverted * elapsedTime
    y += gyConverted * elapsedTime
    z += gzConverted * elapsedTime



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
        rocket.orientation = (x, y, z)
        viewer.update()
