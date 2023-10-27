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
#ifndef CONFIG_BLUEPAD32_PLATFORM_ARDUINO
#error "Must only be compiled when using Bluepad32 Arduino platform"
#endif  // !CONFIG_BLUEPAD32_PLATFORM_ARDUINO

#include <Arduino.h>
#include <Bluepad32.h>

#include <ESP32Servo.h>
#include <ESP32SharpIR.h>
#include <QTRSensors.h>

#define LED 4

GamepadPtr myGamepads[BP32_MAX_GAMEPADS];

ESP32SharpIR sensor1 ( ESP32SharpIR::GP2Y0A21YK0F, 27);

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

 
Servo servo1; // declare objects
Servo servo2;
Servo mecharm;
QTRSensors qtr;

void setup() {
    BP32.setup(&onConnectedGamepad, &onDisconnectedGamepad);
    BP32.forgetBluetoothKeys(); 
    pinMode(LED,OUTPUT);

    servo1.setPeriodHertz(50); // servo expects a pulse ~20 ms
    servo2.setPeriodHertz(50); // servo expects a pulse ~20 ms
    servo1.attach(2, 1000, 2000); // configures pin 13 ususth min/max pulse widths 
    servo2.attach(15, 1000, 2000); // configures pin 13 with min/max pulse widths
    mecharm.attach(0, 1000, 2000);

    Serial.begin(115200);
    sensor1.setFilterRate(0.1f);
    
}



void loop() {
    BP32.update();
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        GamepadPtr controller = myGamepads[i];
        if (controller && controller->isConnected()) {

            /* button testing
             color detection (B)
             line follow (Y)
             wall follow (X)
             normal drive (A)

            l1 - launcher arm clockwise
            r2 - launcher arm counterclockwise
            */
           
            Serial.print(controller->a()); // b on remote
            Serial.print(controller->b()); // a on remote
            Serial.print(controller->x()); // y on remote
            Serial.print(controller->y()); // x on remote
            Serial.print(controller->l1());
            Serial.print(controller->r1());
            

            Serial.print("x-axis: ");
            Serial.print(controller->axisX());
            Serial.print(" y-axis: ");
            Serial.println(controller->axisRY());
            // forward control (RIGHT JOYSTICK)
            servo1.write(((((float) controller->axisRY()) / 512.0f) * 500) + 1500);
            servo2.write(((((float) controller->axisRY()) / 512.0f) * 500) + 1500);

            // turning control (LEFT JOYSTICK)
                // -x
            if (controller->axisX() > 0.0f) {
                servo1.write((-1) * ((((float) controller->axisX()) / 512.0f) * 500) + 1500);
                servo2.write(((((float) controller->axisX()) / 512.0f) * 500) + 1500);
                // Serial.println("1");
            }
                // +x
            if (controller->axisX() < 0.0f) {
                servo1.write(((((float) controller->axisX()) / 512.0f) * 500) + 1500);
                servo2.write((-1) * ((((float) controller->axisX()) / 512.0f) * 500) + 1500);
                // Serial.print("2");
            }

            if (controller->b()) {

                while(1) { // want a while loop that will ALWAYS run
                    BP32.update();
                    Serial.println(controller->a() );

                        digitalWrite(LED,LOW);
                        Serial.println("low");
                        delay(500);
                        digitalWrite(LED,HIGH);
                        Serial.println("high");
                        delay(500);

                    if(controller->a() == 1) {
                        digitalWrite(LED,LOW);
                        break;
                    
                    }
                }
            }
        }
    }



        vTaskDelay(1);

}

//arduino main with more controller stuff