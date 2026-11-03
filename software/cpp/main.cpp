#include <Arduino.h>
#include <Wire.h>
#include <SparkFun_ISM330DHCX.h>
#include <CRC8.h>
#include <cstring>

// ls /dev/ttyACM*

//THINGS TO DO
//NOTHING IVE DONE IT ALL

const uint8_t debugLevel = 2; //0=OFF, 1=LOW sends a summary every second, 2=HIGH sends a packet snapshot every .5 seconds

CRC8 crc(0x07, 0x00, 0x00, false, false);
SparkFun_ISM330DHCX imu;

//global variables
sfe_ism_data_t accelData;
sfe_ism_data_t gyroData;
const uint16_t sensorSampleRate = 400;
uint32_t previousSampleTime;
uint8_t accelScale;
uint16_t gyroScale;
int16_t accelRawScale;
int32_t gyroRawScale;
String booyah = "BOOYAH";

//packet
const uint8_t packetLength = 18;
const uint8_t startByte1 = 0xAA;
const uint8_t startByte2 = 0x55;
uint32_t timestamp;
uint32_t timestampLastSample;
uint8_t packetCounter = 0;
int16_t gx;
int16_t gxLastSample;
int16_t gy;
int16_t gyLastSample;
int16_t gz;
int16_t gzLastSample;
int8_t ax;
int8_t axLastSample;
int8_t ay;
int8_t ayLastSample;
int8_t az;
int8_t azLastSample;
uint8_t telemetryFlags = 0;
uint8_t crcValue;
uint32_t previousPacketTime;
uint8_t packet[packetLength];
const uint8_t packetHz = 100;

//Telemetry Flags
bool READ_ERROR = false;
bool ACCELEROMETER_SATURATED = false;
bool GYROSCOPE_SATURATED = false;
bool STATIONARY = false;
bool STALE = false;
bool gxBURST = false;
bool gyBURST = false;
bool gzBURST = false;

//Health Summary
uint16_t summarySamples = 0;
uint16_t summaryPackets = 0;
uint16_t summaryReadErrors = 0;
uint32_t summaryMaxLoop = 0; //micros
uint32_t summarySampleAge = 0; //micros
uint32_t lastSummaryTime = 0;
uint8_t summaryHZ = 1;
uint32_t maxLoopStart = 0;
bool gyroOK = false;
bool accelOK = false;
const float saturationThreshold = 0.8f;
const float burstThreshold = 0.5f;
const float stationaryThreshold = 0.005f;
const uint32_t staleTimeout = 50000; // micros
bool haveSample = false;

//Advanced Summary
uint32_t lastAdvancedTime = 0;
uint8_t advancedHZ = 2;

void setup() {
  if(debugLevel >= 1){
    Serial.begin(115200);
  }
  while(!Serial && millis() < 3000){
    delay(1);
  }
  if(debugLevel >= 1){
    Serial.println("UART AND I2C STARTUP - ");
  }
  Wire.begin();
  Serial1.begin(19200);

  if(debugLevel >= 1){
    Serial.println(booyah);
  }

  if(debugLevel >= 1){
    Serial.print("IMU STARTUP - ");
  }

  if(!imu.begin(Wire, 0x6a)){
    if(debugLevel >= 1){
      Serial.print("IMU FAILED TO START");
    }

    while(true){
      delay(1);
    }
  }
  if(debugLevel >= 1){
    Serial.println(booyah);
  }
  if(debugLevel >= 1){
    Serial.println("IMU RESETTING - ");
  }
  imu.deviceReset();
  // Wait for it to finish reseting
  while( !imu.getDeviceReset() ){
    delay(1);
  }
  if(debugLevel >= 1){
    Serial.println(booyah);
  }

  if(debugLevel >= 1){
    Serial.println("IMU SETTINGS : ");
  }
  imu.setDeviceConfig();
  imu.setBlockDataUpdate();
  // Set the output data rate and precision of the accelerometer
  imu.setAccelDataRate(ISM_XL_ODR_416Hz);
  imu.setAccelFullScale(ISM_8g);
  accelScale = 8; //G's

  if(debugLevel >= 1){
    Serial.print("ACCEL SCALE - ");
    Serial.print(accelScale);
    Serial.println(" G's");
  }

  accelRawScale = accelScale * 1000;

  // Set the output data rate and precision of the gyroscope
  imu.setGyroDataRate(ISM_GY_ODR_416Hz);
  imu.setGyroFullScale(ISM_500dps);
  gyroScale = 500; //dps

  if(debugLevel >= 1){
    Serial.print("GYRO SCALE - ");
    Serial.print(gyroScale);
    Serial.println(" deg/s");
  }

  gyroRawScale = gyroScale * 1000;

  // Turn on the accelerometer's filter and apply settings.
  imu.setAccelFilterLP2();
  imu.setAccelSlopeFilter(ISM_LP_ODR_DIV_100);

  if(debugLevel >= 1){
    Serial.print("ACCEL FILTERING - ");
    Serial.println("ON : 100");
  }

  // Turn on the gyroscope's filter and apply settings.
  imu.setGyroFilterLP1();
  imu.setGyroLP1Bandwidth(ISM_MEDIUM);

  if(debugLevel >= 1){
    Serial.print("GYRO FILTERING - ");
    Serial.println("ON : MEDIUM");
  }

  previousSampleTime=micros();
  previousPacketTime=micros();

  if(debugLevel >= 1){
    Serial.println("SETUP SUCCESSFUL");
  }
}

void loop() {
  maxLoopStart = micros();
  if(imu.checkStatus()){

    if(micros() - previousSampleTime >= (1000000/sensorSampleRate)){
      accelOK = imu.getAccel(&accelData);
      gyroOK = imu.getGyro(&gyroData);

      if(!accelOK || !gyroOK){
        summaryReadErrors++;
        READ_ERROR = true;
      }

      if(accelOK && gyroOK){
        haveSample = true;
        timestamp = micros();

        gyroData.xData = constrain(gyroData.xData, -(gyroRawScale), gyroRawScale);
        gyroData.yData = constrain(gyroData.yData, -(gyroRawScale), gyroRawScale);
        gyroData.zData = constrain(gyroData.zData, -(gyroRawScale), gyroRawScale);

        accelData.xData = constrain(accelData.xData, -(accelRawScale), accelRawScale);
        accelData.yData = constrain(accelData.yData, -(accelRawScale), accelRawScale);
        accelData.zData = constrain(accelData.zData, -(accelRawScale), accelRawScale);

        float fraction;

        fraction = (accelData.xData / float(accelRawScale));
        if(fraction > saturationThreshold || fraction < -saturationThreshold){
          ACCELEROMETER_SATURATED = true;
        }
        ax = lroundf(fraction * 127.0f);
        fraction = (accelData.yData / float(accelRawScale));
        if(fraction > saturationThreshold || fraction < -saturationThreshold){
          ACCELEROMETER_SATURATED = true;
        }
        ay = lroundf(fraction * 127.0f);
        fraction = (accelData.zData / float(accelRawScale));
        if(fraction > saturationThreshold || fraction < -saturationThreshold){
          ACCELEROMETER_SATURATED = true;
        }
        az = lroundf(fraction * 127.0f);

        fraction = (gyroData.xData / float(gyroRawScale));
        if(fraction > saturationThreshold || fraction < -saturationThreshold){
          GYROSCOPE_SATURATED = true;
        }
        if(fraction > burstThreshold || fraction < -burstThreshold){
          gxBURST = true;
        }
        if(fraction < stationaryThreshold && fraction > -stationaryThreshold){
          STATIONARY = true;
        }else{
          STATIONARY = false;
        }
        gx = lroundf(fraction * 32767.0f);
        fraction = (gyroData.yData / float(gyroRawScale));
        if(fraction > saturationThreshold || fraction < -saturationThreshold){
          GYROSCOPE_SATURATED = true;
        }
        if(fraction > burstThreshold || fraction < -burstThreshold){
          gyBURST = true;
        }
        if(STATIONARY){
          if(fraction < stationaryThreshold && fraction > -stationaryThreshold){
            STATIONARY = true;
          }else{
            STATIONARY = false;
          }
        }
        gy = lroundf(fraction * 32767.0f);
        fraction = (gyroData.zData / float(gyroRawScale));
        if(fraction > saturationThreshold || fraction < -saturationThreshold){
          GYROSCOPE_SATURATED = true;
        }
        if(fraction > burstThreshold || fraction < -burstThreshold){
          gzBURST = true;
        }
        if(STATIONARY){
          if(fraction < stationaryThreshold && fraction > -stationaryThreshold){
            STATIONARY = true;
          }else{
            STATIONARY = false;
          }
        }
        gz = lroundf(fraction * 32767.0f);
        summarySamples++;
      }
      previousSampleTime = micros();
    }
  }

  if(micros() - previousPacketTime >= (1000000 / packetHz)){
    packet[0] = startByte1;
    packet[1] = startByte2;
    memcpy(&packet[2], &timestamp, sizeof(timestamp));
    memcpy(&packet[6], &gx, sizeof(gx));
    memcpy(&packet[8], &gy, sizeof(gy));
    memcpy(&packet[10], &gz, sizeof(gz));
    packet[12] = ax;
    packet[13] = ay;
    packet[14] = az;
    packet[15] = packetCounter;

    STALE = !haveSample || (micros() - timestamp) >= staleTimeout;
    telemetryFlags = 0;

    if (READ_ERROR) telemetryFlags |= (1u << 0);
    if (ACCELEROMETER_SATURATED) telemetryFlags |= (1u << 1);
    if (GYROSCOPE_SATURATED) telemetryFlags |= (1u << 2);
    if (STATIONARY) telemetryFlags |= (1u << 3);
    if (STALE) telemetryFlags |= (1u << 4);
    if (gxBURST) telemetryFlags |= (1u << 5);
    if (gyBURST) telemetryFlags |= (1u << 6);
    if (gzBURST) telemetryFlags |= (1u << 7);

    packet[16] = telemetryFlags;

    crc.restart();
    for(uint8_t i = 2; i <=16; i++){
      crc.add(packet[i]);
    }
    crcValue = crc.calc();

    packet[17] = crcValue;

    Serial1.write(packet, packetLength);

    READ_ERROR = false;
    ACCELEROMETER_SATURATED = false;
    GYROSCOPE_SATURATED = false;
    STATIONARY = false;
    STALE = false;
    gxBURST = false;
    gyBURST = false;
    gzBURST = false;

    axLastSample = ax;
    ayLastSample = ay;
    azLastSample = az;

    gxLastSample = gx;
    gyLastSample = gy;
    gzLastSample = gz;

    timestampLastSample = timestamp;

    previousPacketTime = micros();
    summaryPackets++;
    packetCounter++;
  }
  if(debugLevel >= 1){
    if((millis() - lastSummaryTime) >= (1000/summaryHZ)){
      Serial.println("SUMMARY : ");
      Serial.print("Samples Sent - ");
      Serial.println(summarySamples);
      Serial.print("Packets Sent - ");
      Serial.println(summaryPackets);
      Serial.print("Read Errors - ");
      Serial.println(summaryReadErrors);
      Serial.print("Max Loop Time - ");
      Serial.println(summaryMaxLoop);
      Serial.print("Sample Age - ");
      Serial.println(micros() - timestamp);

      summarySamples = 0;
      summaryPackets = 0;
      summaryReadErrors = 0;
      summaryMaxLoop = 0;

      lastSummaryTime = millis();
    }
  }

  if(debugLevel >= 2)
    if(millis() - lastAdvancedTime > (1000/advancedHZ)){
      Serial.println("ADVANCED PACKET DATA : ");
      Serial.print("Timestamp - ");
      Serial.print(timestampLastSample);
      Serial.println(" microseconds");

      Serial.print("Accel Axes - ");
      Serial.print(" X: ");
      Serial.print(axLastSample);
      Serial.print(",");
      Serial.print(" Y: ");
      Serial.print(ayLastSample);
      Serial.print(",");
      Serial.print(" Z: ");
      Serial.print(azLastSample);
      Serial.println(",");

      Serial.print("Gyro Axes - ");
      Serial.print(" X: ");
      Serial.print(gxLastSample);
      Serial.print(",");
      Serial.print(" Y: ");
      Serial.print(gyLastSample);
      Serial.print(",");
      Serial.print(" Z: ");
      Serial.print(gzLastSample);
      Serial.println(",");

      Serial.print("Flags - ");
      for (int i = 7; i >= 0; i--) {
          Serial.print((telemetryFlags >> i) & 1u);
      }
      Serial.println();

      Serial.print("CRC - 0x");
      Serial.println(crcValue, HEX);

      lastAdvancedTime = millis();
    }
  if((micros() - maxLoopStart) > summaryMaxLoop){
    summaryMaxLoop = (micros() - maxLoopStart);
  }
}