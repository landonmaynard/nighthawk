#include <packet.h>
#include <link.h>
#include <sense.h>

Link link;
GyroPacket gyroPacket;
IMU imu;

void setup(){
    link.initialize(14400);
    if(!imu.initialize(0x6a)){
        delay(1); //add debugging
    }
}

void loop(){
    imu.read();
    if(gyroPacket.isDue(imu.imuData.timestamp)){
        imu.convert();
        gyroPacket.update(imu.imuData.timestamp, imu.imuData.gx, imu.imuData.gy, imu.imuData.gz);
        link.send(gyroPacket.getData(), gyroPacket.getLength()); 
    }
}