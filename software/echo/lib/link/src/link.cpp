#include "link.h"

void Link::initialize(){
    Serial1.begin(19200);
}

void Link::update(){
    packet += 1;
    if(packet >= 6){
        packet = 1;
    }
}

bool Link::send(){
    if(millis() - lastPacketTime >= 1000/packetHZ){
        Serial1.write(packet);
        lastPacketTime = millis();
        return true;
    }else{
        return false;
    }
}