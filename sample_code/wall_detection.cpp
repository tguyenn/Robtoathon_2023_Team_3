#include "sdkconfig.h"
#ifndef CONFIG_BLUEPAD32_PLATFORM_ARDUINO
#error "Must only be compiled when using Bluepad32 Arduino platform"
#endif  // !CONFIG_BLUEPAD32_PLATFORM_ARDUINO

#include <Arduino.h>
#include <Bluepad32.h>

#include <ESP32Servo.h>
#include <ESP32SharpIR.h>
#include <QTRSensors.h>

// current setup only works from range of 9 cm to ~22 cm
// 3 cm to 9 cm outputs 9.00 cm
// 0 cm to 3 cm outputs bad values
// CHANGE VALUES OF SET FILTERRATE TO GET BETTER RESULTS(?)

ESP32SharpIR left(ESP32SharpIR::GP2Y0A21YK0F, 36);

void setup()  {
    Serial.begin(115200);
    left.setFilterRate(0.5f); // works if you adjust filter rate properly (?)
}

void loop() {
    Serial.println(left.getDistanceFloat()); // this function doesnt work for whatever reason idrk why. 
    // it might be because the filter rate was not right??
    // Serial.println(left.getRawDistance()); // works and produces stable output but not in cm or units of measure?
    delay(100);
} 