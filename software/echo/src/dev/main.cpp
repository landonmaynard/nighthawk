#include <packet.h>
#include <link.h>
#include <sense.h>

Link link;
HeartbeatPacket heartbeatPacket;
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
    if(heartbeatPacket.isDue(micros())){
        heartbeatPacket.update();
        link.send(heartbeatPacket.getData(), heartbeatPacket.getLength());
    }
    if(gyroPacket.isDue(imu.imuData.timestamp)){
        imu.convert();
        gyroPacket.update(imu.imuData.timestamp, imu.imuData.gx, imu.imuData.gy, imu.imuData.gz);
        link.send(gyroPacket.getData(), gyroPacket.getLength()); 
    }
}