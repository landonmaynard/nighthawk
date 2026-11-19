//socat -d -d PTY,raw,echo=0,link=/tmp/echo PTY,raw,echo=0,link=/tmp/sonar

#include <fcntl.h>   // open()
#include <unistd.h>  // write(), close()
#include <cstdint>
#include <chrono>
#include <thread>

const uint8_t HZ = 5;
bool increasing = true;

uint8_t packet[4];

const uint16_t altitudeTarget = 500;
const uint8_t batteryPercentage = 76;
const uint8_t temperature = 200;
const uint8_t speed = 5;
uint16_t sample = 0;
uint16_t tempSample = 0;
uint16_t simulateAltitude(uint16_t tempSample);
std::chrono::steady_clock::time_point lastSend = std::chrono::steady_clock::now();

uint16_t simulateAltitude(uint16_t tempSample){ 
    if(increasing == true && tempSample <  altitudeTarget){
        tempSample += speed;
    }
    else if(increasing == true && tempSample >= altitudeTarget){
        increasing = false;
    }
    if(increasing == false && tempSample > 0){
        tempSample -= speed;
    }
    else if(increasing == false && tempSample <= 0){
        increasing = true;
    }
    return tempSample;
}

int main() {
        
    int port = open("/tmp/echo", O_RDWR);

    if (port == -1) {
        return 1;
    }
    while (true) {
        if( (std::chrono::steady_clock::now() - lastSend) > (std::chrono::milliseconds(1000/HZ)) ){
            sample = simulateAltitude(sample);
            packet[0] = sample & 0b0000000011111111;
            packet[1] = sample >> 8;
            packet[2] = batteryPercentage;
            packet[3] = temperature;
            write(port, &packet, sizeof(packet));

            lastSend = std::chrono::steady_clock::now();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}