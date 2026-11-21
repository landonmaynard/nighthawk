#pragma once
#include <stdint.h>
#include "globals.h"
#include <Arduino.h>

class GyroPacket
{
private:

    static constexpr uint8_t packetLength = 15;
    static constexpr uint8_t usefulPacketLength = (packetLength-3);

    const uint8_t id = 0x00;
    const uint8_t packetHZ = 60;
    uint8_t packetCount = 0;
    uint8_t data[packetLength]; //fill in how many

    uint32_t lastTransmission = micros();

    struct TimestampBytes{
        uint8_t byte1;
        uint8_t byte2;
        uint8_t byte3;
        uint8_t byte4;
    };

    struct GyroBytes{
        uint8_t x1;
        uint8_t x2;
        uint8_t y1;
        uint8_t y2;
        uint8_t z1;
        uint8_t z2;
    };

    TimestampBytes convertTimestamp(uint32_t timestamp);
    GyroBytes convertGyro(uint16_t xData, uint16_t yData, uint16_t zData);

    uint8_t calculateCRC(uint8_t data[usefulPacketLength]);

public:
    GyroPacket();
    ~GyroPacket();
    void update(uint32_t timestamp, uint16_t xData, uint16_t yData, uint16_t zData);
    uint8_t getLength();
    const uint8_t* getData() const;
    bool isDue(uint32_t timestamp);
};