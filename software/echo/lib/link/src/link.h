#pragma once
#include <Arduino.h>

class Link
{
private:
    uint8_t packet = 1;
    const uint8_t packetHZ = 100;
    uint32_t lastPacketTime = millis();
public:
    void initialize();
    void update();
    bool send();
};
