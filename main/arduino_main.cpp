/****************************************************************************
Copyright 2021 Ricardo Quesada

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
****************************************************************************/

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

GamepadPtr myGamepads[BP32_MAX_GAMEPADS];

// checks if controller connects with ESP32
void onConnectedGamepad(GamepadPtr gp) {
    bool foundEmptySlot = false;
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myGamepads[i] == nullptr) {
            // Console.printf("CALLBACK: Gamepad is connected, index=%d\n", i);
            // Additionally, you can get certain gamepad properties like:
            // Model, VID, PID, BTAddr, flags, etc.
            // GamepadProperties properties = gp->getProperties();
            // Console.printf("Gamepad model: %s, VID=0x%04x, PID=0x%04x\n", gp->getModelName(), properties.vendor_id,
            //                properties.product_id);
            myGamepads[i] = gp;
            foundEmptySlot = true;
            break;
        }
    }
    if (!foundEmptySlot) {
        // Console.println("CALLBACK: Gamepad connected, but could not found empty slot");
    }
}

void onDisconnectedGamepad(GamepadPtr gp) {
    bool foundGamepad = false;
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myGamepads[i] == gp) {
            // Console.printf("CALLBACK: Gamepad is disconnected from index=%d\n", i);
            myGamepads[i] = nullptr;
            foundGamepad = true;
            break;
        }
    }

    if (!foundGamepad) {
        // Console.println("CALLBACK: Gamepad disconnected, but not found in myGamepads");
    }
}

 
Servo servo1; // declare servo objects
Servo servo2; 
void setup() {
    BP32.setup(&onConnectedGamepad, &onDisconnectedGamepad);
    BP32.forgetBluetoothKeys(); 

    servo1.setPeriodHertz(50); // servo expects a pulse ~20 ms
    servo2.setPeriodHertz(50); // servo expects a pulse ~20 ms
    servo1.attach(13, 1000, 2000); // configures pin 13 ususth min/max pulse widths
    servo2.attach(12, 1000, 2000); // configures pin 13 with min/max pulse widths

}

void loop() {
    BP32.update();
    GamepadPtr controller = myGamepads[0];
    if (controller || controller->isConnected()) {
        // forward control (y-axis)
        servo1.write(((((float) controller->axisY()) / 512.0f) * 500) + 1500);
        servo2.write(((((float) controller->axisY()) / 512.0f) * 500) + 1500);

        // turning control (x-axis)
            // -x
        if (controller->axisX() > 0.0f) {
            servo1.write((-1) * ((((float) controller->axisX()) / 512.0f) * 500) + 1500);
            servo2.write(((((float) controller->axisX()) / 512.0f) * 500) + 1500);
        }
            // +x
            // :D
        if (controller->axisX() < 0.0f) {
            servo1.write(((((float) controller->axisX()) / 512.0f) * 500) + 1500);
            servo2.write((-1) * ((((float) controller->axisX()) / 512.0f) * 500) + 1500);
        }
    }
    vTaskDelay(1);

}
