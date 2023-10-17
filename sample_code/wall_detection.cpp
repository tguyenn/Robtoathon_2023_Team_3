#include "sdkconfig.h"
#ifndef CONFIG_BLUEPAD32_PLATFORM_ARDUINO
#error "Must only be compiled when using Bluepad32 Arduino platform"
#endif  // !CONFIG_BLUEPAD32_PLATFORM_ARDUINO

#include <Arduino.h>
#include <Bluepad32.h>

#include <ESP32Servo.h>
#include <ESP32SharpIR.h>
#include <QTRSensors.h>

ESP32SharpIR left(ESP32SharpIR::GP2Y0A21YK0F, 36);

void setup()  {
    Serial.begin(115200);
    // left.setFilterRate(0.1f); // works if you adjust filter rate properly (?)
}

void loop() {

    Serial.println("test");
    // Serial.println(left.getDistanceFloat()); // this function doesnt work for whatever reason idrk why. 
    // it might be because the filter rate was not right??
    Serial.println(left.getRawDistance()); // works and produces stable output but not in cm or units of measure
    delay(500);
}