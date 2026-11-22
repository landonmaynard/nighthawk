#pragma once
#include <stdint.h>
#include <Arduino.h>
#include <Wire.h>

class Link
{

private:

public:
    void initialize(uint32_t baud);
    void send(const uint8_t* data, uint8_t packetLength);
};