#include "link.h"

void Link::initialize(uint32_t baud){
    Serial1.begin(baud);
    Wire.begin();
}

void Link::send(const uint8_t* data, uint8_t packetLength){
    Serial1.write(data, packetLength);
}
