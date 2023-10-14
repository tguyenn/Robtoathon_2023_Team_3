#include "sdkconfig.h"
#include<iostream>
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
QTRSensors qtr;

void clockwise() {
    Serial.println("clockwise");
}

void counterclockwise() {
    Serial.println("counterclockwise");
}




void setup() {
// set up Serial Communication and sensor pins 
Serial.begin(115200);
qtr.setTypeRC(); // or setTypeAnalog() 
qtr.setSensorPins ((const uint8_t []) {12, 14, 27}, 3);

// calibration sequence
    for (uint8_t i = 0; i < 250; i++) {
        Serial.println("calibrating"); 
        qtr.calibrate();
        delay(20);
    }
}


void loop() {
    uint16_t sensors[2];
    int16_t position = qtr.readLineBlack(sensors);
    
    Serial.print("sensor1: ");
    Serial.println(sensors[0]);
    Serial.print("sensor2: ");
    Serial.println(sensors[1]);
    Serial.print("sensor3: ");
    Serial.println(sensors[2]);
    Serial.println(position);

    // 1000 means ON LINE OR EXPOSED TO MAX BRIGHTNESS
    if(sensors[0] > 500 && sensors[2] < 500) {
        clockwise();
    }

    if(sensors[0] < 500 && sensors[2] > 500) {
        counterclockwise();
    }

    delay(100);
}
