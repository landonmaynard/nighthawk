#include <Arduino.h>
#include <link.h>

Link link;

void setup(){
    link.initialize();
}

void loop(){
    if(link.send()){
        link.update();
    }
}