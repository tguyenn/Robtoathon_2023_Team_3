#include "sdkconfig.h"
#ifndef CONFIG_BLUEPAD32_PLATFORM_ARDUINO
#error "Must only be compiled when using Bluepad32 Arduino platform"
#endif  // !CONFIG_BLUEPAD32_PLATFORM_ARDUINO
#include <Arduino.h>
#include <Bluepad32.h>

#include <ESP32Servo.h>
#include <ESP32SharpIR.h>
#include <QTRSensors.h>


GamepadPtr myGamepads[BP32_MAX_GAMEPADS];
ESP32SharpIR sensor1 ( ESP32SharpIR::GP2Y0A21YK0F, 27);
Servo servo1;
Servo servo2;
Servo mecharm;
QTRSensors qtr;


// BP32 FUNCTIONS
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

// WALL FOLLOW FUNCTIONS

// COLOR DETECTION FUNCTIONS

// LINE FOLLOW FUNCTIONS
void clockwise() {
    Serial.println("clockwise clockwise clockwise clockwise clockwise");
    servo1.write(1750);
    servo2.write(1500);
}

void counterclockwise() {
    Serial.println("counterclockwise counterclockwise counterclockwise counterclockwise counterclockwise");
    servo1.write(1500);
    servo2.write(1750);
}

void straight() {
    Serial.println("straight straight straight straight straight straight");
    servo1.write(1750); 
    servo2.write(1750);
}

void lineSetup() {  
    qtr.setTypeRC(); // or setTypeAnalog() 
    qtr.setSensorPins ((const uint8_t []) {12, 14, 27, 26}, 4);
    // int counter;
    // calibration sequence
    // TO DO: ADD SWEEPING MOTION SO NO JANKY CALIBRATION BY HAND
        for (uint8_t i = 0; i < 250; i++) {
            servo1.write(1750);
            // int counter = 30;
            // if(i == counter) {
            //     counter == i + 30;

            // }
            Serial.println("calibrating"); 
            qtr.calibrate();
            delay(20);
        }  
}

void lineLoop() {
    uint16_t sensors[4];
    int16_t position = qtr.readLineBlack(sensors);
    // set thresholds easily
    int lower, upper;
    lower = 600; 
    upper = 900; 

    Serial.print("sensor1: ");
    Serial.println(sensors[0]);
    Serial.print("sensor2: ");
    Serial.println(sensors[1]);
    Serial.print("sensor3: ");
    Serial.println(sensors[2]);
    Serial.print("sensor4: ");
    Serial.println(sensors[3]);

    delay(100); // might need to disable for comp

    // 900+ means LINE 
    // 300 or less means NO LINE
    // I represents line, o represents no line
    
    // Iooo
    if(sensors[0] > upper && sensors[1] < lower && sensors[2] < lower && sensors[3] < lower ) {
        counterclockwise();
    }

    // oooI
    if(sensors[0] < lower && sensors[1] < lower && sensors[2] < lower && sensors[3] > upper) {
        clockwise();
    }

    // oIIo
    if(sensors[0] < lower && sensors[1] > upper && sensors[2] > upper && sensors[3] < lower) {
        straight();
    }

    // oooo
    if(sensors[0] < 300 && sensors[1] < 300 && sensors[2] < 300 && sensors[3] < 300) {
        // SPIN CLOCKWISE IN PLACE
        Serial.println("error");
        servo1.write(1750);
        servo2.write(1250);
    }
}



// MAIN SETUP AND LOOP

void setup() {
    BP32.setup(&onConnectedGamepad, &onDisconnectedGamepad);
    BP32.forgetBluetoothKeys(); 

    servo1.setPeriodHertz(50);
    servo2.setPeriodHertz(50);
    servo1.attach(2); 
    servo2.attach(15); 
    mecharm.attach(0);

    Serial.begin(115200);
    sensor1.setFilterRate(0.1f);
    
}

void loop() {
    BP32.update();
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        GamepadPtr controller = myGamepads[i];
        if (controller && controller->isConnected()) {

            /* ACTUAL BUTTONS
             line follow (A)
             color detection (Y)
             wall follow (X)
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
            Serial.print("x-axis: ");
            Serial.print(controller->axisX());
            Serial.print(" y-axis: ");
            Serial.println(controller->axisRY());


            // forward/backward control (RIGHT JOYSTICK Y-AXIS)
            servo1.write(((((float) controller->axisRY()) / 512.0f) * 500) + 1500);
            servo2.write(((((float) controller->axisRY()) / 512.0f) * 500) + 1500);

            // turning control (LEFT JOYSTICK X-AXIS)
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


            // LINE FOLLOW (PHYSICAL BUTTON A)
            if (controller->b()) { 
                lineSetup();

                while(1) {
                    BP32.update();
                    Serial.println(controller->a() );

                    lineLoop();

                    if(controller->a() == 1) {
                        break;
                    }
                }
            }

            // WALL FOLLOW (PHYSICAL BUTTON X probably)
            if (controller->x()) {
                Serial.print("WALL FOLLOWING");
                while(1) {
                    BP32.update();
                    // loop code here


                    if(controller->a() == 1) {
                        break;
                    }
                }
            }

            // COLOR DETECTION (PHYSICAL BUTTON Y probably)
            if (controller->y()) {
                Serial.print("COLOR SAMPLING");
                while(1) {
                    BP32.update();

                    if(controller->a() == 1) {
                        break;
                    // setup and loop code here
                    }
                }
            }
    }
        vTaskDelay(1);
    }
}