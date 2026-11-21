#include "sense.h"

bool IMU::initialize(uint8_t address){
    if(!sensor.begin(Wire, address)){
        return false;
    }
    sensor.deviceReset();
    while(!sensor.getDeviceReset()){
        delay(1);
    }
    IMU::config();

}

void IMU::config(){
    sensor.setDeviceConfig();
    sensor.setBlockDataUpdate();
    sensor.setAccelDataRate(ISM_XL_ODR_416Hz);
    sensor.setAccelFullScale(ISM_8g);
    sensor.setGyroDataRate(ISM_GY_ODR_416Hz);
    sensor.setGyroFullScale(ISM_500dps);
    sensor.setAccelFilterLP2();
    sensor.setAccelSlopeFilter(ISM_LP_ODR_DIV_100);
}

void IMU::read(){
    if(IMU::available()){
        sensor.getAccel(&accelData);
        sensor.getGyro(&gyroData);
        imuData.timestamp = micros();
    }
}

bool IMU::available(){
    return sensor.checkStatus();
}

void IMU::convert(){
    float limited;
    limited = constrain(accelData.xData, -(accelRange*1000.0f), (accelRange*1000.0f));
    imuData.ax = (int16_t)roundf(limited / (accelRange * 1000.0f) * 32767.0f);
    limited = constrain(accelData.yData, -(accelRange*1000.0f), (accelRange*1000.0f));
    imuData.ay = (int16_t)roundf(limited / (accelRange * 1000.0f) * 32767.0f);
    limited = constrain(accelData.zData, -(accelRange*1000.0f), (accelRange*1000.0f));
    imuData.az = (int16_t)roundf(limited / (accelRange * 1000.0f) * 32767.0f);
    limited = constrain(gyroData.xData, -(gyroRange*1000.0f), (gyroRange*1000.0f));
    imuData.gx = (int16_t)roundf(limited / (gyroRange * 1000.0f) * 32767.0f);
    limited = constrain(gyroData.yData, -(gyroRange*1000.0f), (gyroRange*1000.0f));
    imuData.gy = (int16_t)roundf(limited / (gyroRange * 1000.0f) * 32767.0f);
    limited = constrain(gyroData.zData, -(gyroRange*1000.0f), (gyroRange*1000.0f));
    imuData.gz = (int16_t)roundf(limited / (gyroRange * 1000.0f) * 32767.0f);
}