#include "packet.h"
#include <CRC.h>

GyroPacket::GyroPacket(){

}

GyroPacket::~GyroPacket(){

}

void GyroPacket::update(uint32_t timestamp, int16_t xData, int16_t yData, int16_t zData){
    data[0] = startByte1;
    data[1] = startByte2;
    data[2] = id;
    TimestampBytes timestampBytes = convertTimestamp(timestamp);
    data[3] = timestampBytes.byte1;
    data[4] = timestampBytes.byte2;
    data[5] = timestampBytes.byte3;
    data[6] = timestampBytes.byte4;
    GyroBytes gyroBytes = convertGyro(xData, yData, zData);
    data[7] = gyroBytes.x1;
    data[8] = gyroBytes.x2;
    data[9] = gyroBytes.y1;
    data[10] = gyroBytes.y2;
    data[11] = gyroBytes.z1;
    data[12] = gyroBytes.z2;
    data[13] = packetCount++;
    data[14] = calculateCRC(data);
}

GyroPacket::TimestampBytes GyroPacket::convertTimestamp(uint32_t timestamp){
    TimestampBytes result;
    result.byte1 = timestamp >> 24;
    result.byte2 = (timestamp >> 16) & 0xFF;
    result.byte3 = (timestamp >> 8) & 0xFF;
    result.byte4 = timestamp & 0xFF;
    return result;
}

GyroPacket::GyroBytes GyroPacket::convertGyro(int16_t xData, int16_t yData, int16_t zData){
    GyroBytes result;
    result.x1 = xData >> 8;
    result.x2 = xData & 0xFF;
    result.y1 = yData >> 8;
    result.y2 = yData & 0xFF;
    result.z1 = zData >> 8;
    result.z2 = zData & 0xFF;
    return result;
}

uint8_t GyroPacket::calculateCRC(uint8_t data[]){
    uint8_t checksum = CRC::Calculate(
        &data[2],
        usefulPacketLength,
        CRC::CRC_8()
    );
    return checksum;
}

const uint8_t* GyroPacket::getData() const {
    return data;
}

uint8_t GyroPacket::getLength(){
    return packetLength;
}

bool GyroPacket::isDue(uint32_t timestamp){
    if(timestamp - lastTransmission >= (1000000UL/packetHZ)){
        lastTransmission = timestamp;
        return true;
    }
    return false;
}