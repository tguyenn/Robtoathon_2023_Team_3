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
    servo1.write(1000);
    Serial.println("clockwise");
}

void counterclockwise() {
    Serial.println("counterclockwise");

}

void straight() {
    Serial.println("straight");
}




void setup() {
// set up Serial Communication and sensor pins 
Serial.begin(115200);
qtr.setTypeRC(); // or setTypeAnalog() 
qtr.setSensorPins ((const uint8_t []) {12, 14, 27, 26}, 4);

servo1.attach(15, 1000, 2000);
servo2.attach(2, 1000, 2000);
// calibration sequence
    for (uint8_t i = 0; i < 250; i++) {
        Serial.println("calibrating"); 
        qtr.calibrate();
        delay(20);
    }
}


void loop() {
    uint16_t sensors[3];
    int16_t position = qtr.readLineBlack(sensors);
    
    Serial.print("sensor1: ");
    Serial.println(sensors[0]);
    Serial.print("sensor2: ");
    Serial.println(sensors[1]);
    Serial.print("sensor3: ");
    Serial.println(sensors[2]);
    Serial.print("sensor4: ");
    Serial.println(sensors[3]);


    // 1000 means ON LINE OR EXPOSED TO MAX BRIGHTNESS
    if(sensors[0] <((sensors[1]+sensors[2]+sensors[3])/3)) {
        clockwise();
    }

    if(sensors[0] < 500 && sensors[2] > 500) {
        counterclockwise();
    }

    delay(100);
}