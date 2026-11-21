#pragma once
#include <stdint.h>
#include <Arduino.h>
#include <SparkFun_ISM330DHCX.h>
#include <Wire.h>

class IMU
{
private:

    SparkFun_ISM330DHCX sensor;
    sfe_ism_data_t gyroData;
    sfe_ism_data_t accelData;
    uint32_t accelRange = 8; //G's change with updates
    uint32_t gyroRange = 500; //d/s change with updates

    bool available();
    void config();

public:

    struct IMUData{
        uint32_t timestamp;
        int16_t ax;
        int16_t ay;
        int16_t az;
        int16_t gx;
        int16_t gy;
        int16_t gz;
    };

    IMUData imuData;
    bool initialize(uint8_t address);
    void read();
    void convert();
};