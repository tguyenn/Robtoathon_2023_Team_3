#include "sdkconfig.h"
#ifndef CONFIG_BLUEPAD32_PLATFORM_ARDUINO
#error "Must only be compiled when using Bluepad32 Arduino platform"
#endif  // !CONFIG_BLUEPAD32_PLATFORM_ARDUINO

#include <Arduino.h>
#include <Bluepad32.h>

#include <ESP32Servo.h>
#include <ESP32SharpIR.h>
#include <QTRSensors.h>

Servo servo1; // declare servo objects
Servo servo2;

ESP32SharpIR front(ESP32SharpIR::GP2Y0A21YK0F, 36);
ESP32SharpIR right(ESP32SharpIR::GP2Y0A21YK0F, 39);
ESP32SharpIR left(ESP32SharpIR::GP2Y0A21YK0F, 34);

void setup()  {
    Serial.begin(115200);
    front.setFilterRate(0.5f);
    left.setFilterRate(0.5f);
    right.setFilterRate(0.5f);
    servo1.attach(15);
    servo2.attach(2);
}

// servo 1 ( left side ) <1500 foward, >1500 backwards
// servo 2 ( right side ) >1500 foward, <1500 backwards

void loop() {
    Serial.print("Front Sensor: ");
    Serial.println(front.getDistanceFloat());
    Serial.print("Left Sensor: "); 
    Serial.println(left.getDistanceFloat());
    Serial.print("Right Sensor: ");
    Serial.println(right.getDistanceFloat());

    if(front.getDistanceFloat() >= 10.00){
        Serial.println("Continue Straight");
        servo1.writeMicroseconds(1000);
        servo2.writeMicroseconds(2000);
    }
    else if((front.getDistanceFloat() < 10.00)&&(right.getDistanceFloat() >= 10.00)&&(left.getDistanceFloat() < 10.00)) {
        Serial.println("Turn Right");
        servo1.writeMicroseconds(1000);
        servo2.writeMicroseconds(1000);
    }   
    else if((front.getDistanceFloat() < 10.00) && (left.getDistanceFloat() >= 10.00) && (right.getDistanceFloat() >= 10.00)) {
        Serial.println("Turn Left");
        servo1.writeMicroseconds(2000);
        servo2.writeMicroseconds(2000);
    }
    else{
        Serial.println("Stop");
        servo1.writeMicroseconds(1500);
        servo2.writeMicroseconds(1500);
    }
} 
