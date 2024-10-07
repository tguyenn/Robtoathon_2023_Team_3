#include "sdkconfig.h"
#ifndef CONFIG_BLUEPAD32_PLATFORM_ARDUINO
#error "Must only be compiled when using Bluepad32 Arduino platform"
#endif  // !CONFIG_BLUEPAD32_PLATFORM_ARDUINO
#include <Arduino.h>
#include <Bluepad32.h>

#include <ESP32Servo.h>
#include <ESP32SharpIR.h>
#include <QTRSensors.h>

#include <Wire.h>
#include <Arduino_APDS9960.h>
#include <bits/stdc++.h>


//color sensor definitions
#define APDS9960_INT 2
#define I2C_SDA 21
#define I2C_SCL 22
#define I2C_FREQ 100000
#define RLED 16
#define GLED 5
#define BLED 17
#define LED 2

#define IN1 19
#define IN2 18
#define IN3 5 
#define IN4 17
#define ENA1 4
#define ENA2 0

GamepadPtr myGamepads[BP32_MAX_GAMEPADS];
ESP32SharpIR front(ESP32SharpIR::GP2Y0A21YK0F, 36);
ESP32SharpIR right(ESP32SharpIR::GP2Y0A21YK0F, 39);
ESP32SharpIR left(ESP32SharpIR::GP2Y0A21YK0F, 34);
Servo mecharm;
QTRSensors qtr;

//color sensor unit & I2C unit
TwoWire I2C_0 = TwoWire(0);
APDS9960 apds = APDS9960(I2C_0, APDS9960_INT);

// MAIN SETUP AND LOOP

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

void setup() {
    BP32.setup(&onConnectedGamepad, &onDisconnectedGamepad);
    BP32.forgetBluetoothKeys(); 
 
    mecharm.attach(15);

    // line sensor
    I2C_0.begin(I2C_SDA, I2C_SCL, I2C_FREQ);
    apds.setInterruptPin(APDS9960_INT);
    apds.begin();

    // line sensor
    qtr.setTypeAnalog(); // or setTypeAnalog()
    qtr.setSensorPins((const uint8_t[]) {12, 14, 27, 26}, 4); // pin numbers go in the curly brackets {}, and number of pins goes after

    // IR Sensor
    front.setFilterRate(0.1f);
    left.setFilterRate(0.1f);
    right.setFilterRate(0.1f);

    pinMode(19, OUTPUT); // output to motor controller
    pinMode(18, OUTPUT); // output to motor controller
    pinMode(5, OUTPUT); // output to motor controller
    pinMode(17, OUTPUT); // output to motor controller
    
    Serial.begin(115200);
}

void loop() {
    BP32.update();
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        GamepadPtr controller = myGamepads[i];
        if (controller && controller->isConnected()) {

            /* ACTUAL BUTTONS
             line follow (A)
             color detection (X)
             wall follow (Y)
             normal drive (B)

            launcher arm clockwise (L1)
            launcher arm counterclockwise (R1)
            */
           
           
           // controller output to serial monitor
            Serial.print(controller->a()); 
            Serial.print(controller->b()); 
            Serial.print(controller->x()); 
            Serial.print(controller->y()); 
            Serial.print(controller->l1());
            Serial.print(controller->r1());
            Serial.print(" Lx-axis: ");
            Serial.print(controller->axisX());
            Serial.print(" Ry-axis: ");
            Serial.println(controller->axisRY());

            if (controller->l1() == 1) {
                Serial.print(" servo move 1 ");
                mecharm.write(1250);
                delay(100);
            }

            if (controller->r1() == 1) {
                Serial.print(" servo move 2 ");
                mecharm.write(1750);
                delay(100);
            }
            
            if(controller->axisRY() > 0) { // negative y is up on stick
                Serial.print(" motor 1 a ");
                digitalWrite(19, LOW);
                digitalWrite(18, HIGH);
            }
            if(controller->axisRY() < 0) {
                Serial.print(" motor 1 b ");
                digitalWrite(19, HIGH);
                digitalWrite(18, LOW);
            }
            if(controller->axisRY() == 0) { // stop motor 1
                digitalWrite(19, LOW);
                digitalWrite(18, LOW);
            }
            if(controller->axisX() > 0) { 
                Serial.print(" motor 2 a ");
                digitalWrite(5, LOW);
                digitalWrite(17, HIGH);
            }
            if(controller->axisX() < 0) { 
                Serial.print(" motor 2 b ");
                digitalWrite(5, HIGH);
                digitalWrite(17, LOW);
            }
            if(controller->axisX() == 0) { // stop motor 2
                digitalWrite(5, LOW);
                digitalWrite(17, LOW);
            }

            // LINE FOLLOW (PHYSICAL BUTTON A)
            if (controller->b()) { 
                Serial.println("LINE FOLLOW");
                uint16_t sensors[4];
                for (uint8_t i = 0; i < 75; i++) { 
                    Serial.println("calibrating");
                    qtr.calibrate(); 
                    delay(20);
                }
                while(1) {
                    BP32.update();
                    if(controller->a() == 1) {
                        break;
                    }
                    qtr.readLineBlack(sensors); // Get calibrated sensor values returned in the sensors array
                    Serial.print(sensors[0]);
                    Serial.print(" ");
                    Serial.print(sensors[1]);
                    Serial.print(" ");
                    Serial.print(sensors[2]);
                    Serial.print(" ");
                    Serial.println(sensors[3]);
                    delay(250);
                }
            }

            // WALL FOLLOW (PHYSICAL BUTTON Y)
            if (controller->x()) {
                Serial.println("WALL FOLLOWING");
                while(1) {
                    BP32.update();
                    if(controller->a() == 1) {
                        break;
                    }
                    Serial.print(" Front Sensor: "); 
                    Serial.print(front.getDistanceFloat());
                    Serial.print(" Left Sensor: "); 
                    Serial.print(left.getDistanceFloat());
                    Serial.print(" Right Sensor: "); 
                    Serial.println(right.getDistanceFloat());
                }
            }

            // COLOR DETECTION (PHYSICAL BUTTON X)
            if (controller->y()) {
                int r, g, b, a;
                // Wait until color is read from the sensor 
                while(1) {
                    BP32.update();
                    if(controller->a() == 1) {
                        break;
                    } 
                    while (!apds.colorAvailable()) { delay(5); }
                    apds.readColor(r, g, b, a);
                    // Read color from sensor apds.readColor(r, g, b, a);
                    // Print color in decimal 
                    Serial.print("RED: ");
                    Serial.print(r);
                    Serial.print(" GREEN: ");
                    Serial.print(g);
                    Serial.print(" BLUE: ");
                    Serial.print(b);
                    Serial.print(" AMBIENT: ");
                    Serial.println(a);
                    delay(100);
                    BP32.update();
                }
            }
        }
        vTaskDelay(1);
    }
}             