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
#define GLED 17
#define BLED 5
#define LED 2
GamepadPtr myGamepads[BP32_MAX_GAMEPADS];
ESP32SharpIR sensor1 ( ESP32SharpIR::GP2Y0A21YK0F, 27); // what is this for???
ESP32SharpIR front(ESP32SharpIR::GP2Y0A21YK0F, 36);
ESP32SharpIR right(ESP32SharpIR::GP2Y0A21YK0F, 39);
ESP32SharpIR left(ESP32SharpIR::GP2Y0A21YK0F, 34);
Servo servo1;
Servo servo2;
Servo mecharm;
QTRSensors qtr;

//color sensor unit & I2C unit
TwoWire I2C_0 = TwoWire(0);
APDS9960 apds = APDS9960(I2C_0, APDS9960_INT);

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

void delay_custom(int wait_val) {
    int i = 0;
    while (i < wait_val) {
        i+=1;
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


// COLOR DETECTION FUNCTIONS

void color_setup() {
    //sets up I2C protocol
    I2C_0.begin(I2C_SDA, I2C_SCL, I2C_FREQ);

    //sets up color sensor
    apds.setInterruptPin(APDS9960_INT);
    apds.begin();
    Serial.begin(115200);
    servo1.attach(15); 
    servo2.attach(2); 
}

// WALL FOLLOW FUNCTIONS

void wall_setup()  {
    front.setFilterRate(0.1f);
    left.setFilterRate(0.1f);
    right.setFilterRate(0.1f);
    servo1.attach(15);
    servo2.attach(2);
}

void Wall_Follow() {
    wall_setup();
    Serial.print("Front Sensor: ");
    Serial.println(front.getDistanceFloat());
    Serial.print("Left Sensor: "); 
    Serial.println(left.getDistanceFloat());
    Serial.print("Right Sensor: ");
    Serial.println(right.getDistanceFloat());


    if(front.getDistanceFloat() >= 15.00){
        Serial.println("Continue Straight");
        servo1.writeMicroseconds(1000);
        servo2.writeMicroseconds(2000);
        delay(100);
    }
    else {
        Serial.println("Turn Right");
        servo1.writeMicroseconds(1000);
        servo2.writeMicroseconds(1000);
        delay(660);
    }
    // else if((right.getDistanceFloat() >= 15.00)&&(left.getDistanceFloat() >= 15.00)){
    //     Serial.println("Turn Right 2");
    //     servo1.writeMicroseconds(500);
    //     servo2.writeMicroseconds(500);
    //     delay_custom(750);
    // }
    // else if((front.getDistanceFloat() < 15.00)&&(right.getDistanceFloat() >= 15.00)&&(left.getDistanceFloat() < 15.00)) {
    //     Serial.println("Turn Right");
    //     servo1.writeMicroseconds(1000);
    //     servo2.writeMicroseconds(1000);
    //     delay_custom(550);
    // }   
    // else if((front.getDistanceFloat() < 15.00) && (left.getDistanceFloat() >= 15.00) && (right.getDistanceFloat() < 15.00)) {
    //     Serial.println("Turn Left");
    //     servo1.writeMicroseconds(2500);
    //     servo2.writeMicroseconds(2500);
    //     delay_custom(500);
    // }
    // else{
    //     Serial.println("Stop");
    //     servo1.write(1500);
    //     servo2.write(1500);
    // }
} 

char color_detector(int r, int g, int b, int a) {

    //this chunk of code is the code that has us identify whatever color we are CURRENTLY on.
    //It is NOT for long term storage of the sample color. 
    int value=r;
    char char_to_return = 'r';
    if (g>value) {
        value = g;
        char_to_return = 'g';
    }
    if (b > value) {
        value = b;
        char_to_return = 'b';
    }

    //currently doing nothing with clear
    return char_to_return;
}
void color_challenge() {
    color_setup();
    //This counter is just to keep track of time. 
    int counter = 0;
    //this boolean says if we have found the final color or not. 
    //Once it's set to true, the while loop will end and so will the function. 
    bool final_color_found = false;

    //this bool says if we have found any new colors besides the original one 
    bool new_color_found = false;

    //this guy holds whatever the sample color is
    char sample_color = 'z';

    // these guys are helper variables for the color_detector function
    int r, g, b, a;

    //this guy holds whatever color we are on rn. 
    char last_color_detected = 'r';
    
    //this guy helps with the part of the code that flashes a certain number of times 
    //to signal what color the sample was
    bool flash_has_happened = false;
    // int test_val=1000;
    while (final_color_found == false) {
        //This thing needs to run right at the beginning it's to initialize the color or something idk
        while (!apds.colorAvailable()) {
            delay(5);
        }
        //these two lines help with the function that determines what color we are currently on.
        apds.readColor(r, g, b, a); //assigns color ints a value based on sensor
        last_color_detected = color_detector(r,g,b,a); //takes in color ints and returns whatever the strongest one is

        //This if statement will run a few times right at the beginning of our program
        //it's purpose is to identify what the sample color that we need to store is.
        //this statement should not be running at all after the first couple seconds
        if ((counter>50) && (counter<90))
        {
            sample_color = last_color_detected;
            Serial.println("The sample color is...");
            Serial.println(sample_color);
        }

        //rest of the time we need to be moving forward and scanning what color we are on
        //THIS LOGIC ASSUMES THAT, ON THE COLOR STRIP, THE COLOR WE NEED TO STOP ON IS NOT
        //IMMEDIATELY AFTER THE SAMPLE COLOR. IF IT IS WE ARE GONNA HAVE ISSUES ):

        //if counter>100 that means it's go time baby. we gotta MOVE
        if (counter>100 ) {

            //this if statement runs ONE TIME. It flashes a few times to let the user know what sample color
            //it thinks is correct.
            if (flash_has_happened == false) {

                if (sample_color == 'r') {
                    //do red flash
                    digitalWrite(LED,LOW);
                    delay(1000);
                    digitalWrite(LED,HIGH);
                    delay(1000);
                    digitalWrite(LED,LOW);
                    delay(1000);
                    Serial.println("SAMPLE COLOR FINAL IS RED");

                }
                else if (sample_color == 'g') {
                    //do green flash
                    digitalWrite(LED,LOW);
                    delay(1000);
                    digitalWrite(LED,HIGH);
                    delay(1000);
                    digitalWrite(LED,LOW);
                    delay(1000);
                    digitalWrite(LED,HIGH);
                    delay(1000);
                    digitalWrite(LED,LOW);
                    delay(1000);
                    Serial.println("SAMPLE COLOR FINAL IS GREEN");
                }
                else if (sample_color == 'b') {
                    //do blue flash
                    digitalWrite(LED,LOW);
                    delay(1000);
                    digitalWrite(LED,HIGH);
                    delay(1000);
                    digitalWrite(LED,LOW);
                    delay(1000);
                    digitalWrite(LED,HIGH);
                    delay(1000);
                    digitalWrite(LED,LOW);
                    delay(1000);
                    digitalWrite(LED,HIGH);
                    delay(1000);
                    digitalWrite(LED,LOW);
                    delay(1000);
                    Serial.println("SAMPLE COLOR FINAL IS BLUE");
                }

                flash_has_happened = true;
            }
            

            //move forward continuously
           
            servo1.writeMicroseconds(1000);
            servo2.writeMicroseconds(2000);

            Serial.println(last_color_detected);
            if ((last_color_detected == sample_color) && (new_color_found == true)) {

                Serial.println("I HAVE FOUND A COLOR MATCHING THE SAMPLE COLOR");
                final_color_found = true; 
                delay(100);
                //END PROGRAM

            }
            //This should trigger as soon as we run into a new color 
            else if ((last_color_detected != sample_color) && (new_color_found == false)) {
                new_color_found = true;
                Serial.println("I am JUST NOW making it off of the sample color");
            }
            //This should be triggering if we are actively traversing and we are on a new color 
            // but it's not the one we want to stop on
            else if ((last_color_detected != sample_color) && (new_color_found== true)) {
                Serial.println("I am off of the sample color.");
            }
            //this should be triggering if we are still on the sample color
            else if ((last_color_detected == sample_color) && ( new_color_found == false)) {
                Serial.println("I am still on the sample color!");
            }

        }
        counter+=1; 
    }



}
// LINE FOLLOW FUNCTIONS
void clockwise() {
    Serial.println("clockwise clockwise clockwise clockwise clockwise");
    servo1.write(1250);
    servo2.write(1500);
}

void counterclockwise() {
    Serial.println("counterclockwise counterclockwise counterclockwise counterclockwise counterclockwise");
    servo1.write(1500);
    servo2.write(1750);
}

void straight() {
    Serial.println("straight straight straight straight straight straight");
    servo1.write(1250); 
    servo2.write(1750);
}

void lineSetup() {  
    qtr.setTypeRC(); 
    qtr.setSensorPins ((const uint8_t []) {12, 14, 27, 26}, 4);
    // calibration sequence
    // TO DO: ADD SWEEPING MOTION SO NO JANKY CALIBRATION BY HAND
        for (uint8_t i = 0; i < 250; i++) {            
            Serial.print("calibrating"); 
            Serial.println(i);
            qtr.calibrate();
            delay(20);
            // NEED TO FIND WAY TO DEFINE CONTROLLER OBJECT GLOBALLY TO CANCEL
            // if(controller->a() == 1) {
            //             break;
            // }
        }  
}

void lineLoop() {
    uint16_t sensors[4];
    int16_t position = qtr.readLineBlack(sensors);
    position = position + 1; // gets rid of annoying warning... idk why we need this value lol
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

    delay(20); // might need to disable for comp

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
        // Serial.println("error");
        // servo1.write(1750);
        // servo2.write(1750);
    }
}



// MAIN SETUP AND LOOP

void setup() {
    BP32.setup(&onConnectedGamepad, &onDisconnectedGamepad);
    BP32.forgetBluetoothKeys(); 

    // servo1.setPeriodHertz(50);
    // servo2.setPeriodHertz(50);
    servo1.attach(15); 
    servo2.attach(2); 
    mecharm.attach(4);

    Serial.begin(115200);
    sensor1.setFilterRate(0.1f); // what is this for??

    front.setFilterRate(0.1f);

    pinMode(RLED,OUTPUT);
    pinMode(GLED,OUTPUT);
    pinMode(BLED,OUTPUT);

    //led setup
    //pinMode(LED,OUTPUT);
    
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
            Serial.print(" x-axis: ");
            Serial.print(controller->axisX());
            Serial.print(" y-axis: ");
            Serial.println(controller->axisRY());

            
            Serial.print("Front Sensor: ");
            Serial.println(front.getDistanceFloat());

            mecharm.write(1500);

            analogWrite(RLED, 100);
            analogWrite(GLED, 100);
            analogWrite(BLED, 100);
            // delay(100);


            // analogWrite(RLED, 0);
            // analogWrite(GLED, 100);
            // analogWrite(BLED, 0);
            // delay(1000);


            // analogWrite(RLED, 0);
            // analogWrite(GLED, 0);
            // analogWrite(BLED, 100);
            // delay(100);

            // if(front.getDistanceFloat() > 10 && front.getDistanceFloat() < 20) {
            //     analogWrite(RLED, 200);
            //     analogWrite(GLED, 0);
            //     analogWrite(BLED, 200);
            //     // delay(300);

            //     if(front.getDistanceFloat() > 14 && front.getDistanceFloat() < 16) {
            //         analogWrite(RLED, 200);
            //         analogWrite(GLED, 200);
            //         analogWrite(BLED, 200);
            //         // delay(100);
            //     }
            // }

            if (controller->l1() == 1) {
                // while(1) {
                //     mecharm.write(1250);
                //     if(controller->r2() == 0) {
                //         break;
                //     }
                // }
                mecharm.write(1250);
                delay(100);
            }

            if (controller->r1() == 1) {
                // while(1) {
                //     mecharm.write(1750);
                //     if(controller->r1() == 0) {
                //         break;
                //     }
                // }
                mecharm.write(1750);
                delay(100);
            }

            // forward/backward control (RIGHT JOYSTICK Y-AXIS)
            servo1.write(-1.0 * ((((float) controller->axisRY()) / 512.0f) * 500) + 1500);
            servo2.write(((((float) controller->axisRY()) / 512.0f) * 500) + 1500);

            // turning control (LEFT JOYSTICK X-AXIS)
                // -x
            if (controller->axisX() > 0.0f) {
                analogWrite(RLED, 0);
                analogWrite(GLED, 200);
                analogWrite(BLED, 0);
                servo1.write(((((float) controller->axisX()) / 512.0f) * 500) + 1500);
                servo2.write(((((float) controller->axisX()) / 512.0f) * 500) + 1500);
            }
                // +x
            if (controller->axisX() < 0.0f) {
                analogWrite(RLED, 0);
                analogWrite(GLED, 0);
                analogWrite(BLED, 200);
                servo1.write(((((float) controller->axisX()) / 512.0f) * 500) + 1500);
                servo2.write(((((float) controller->axisX()) / 512.0f) * 500) + 1500);
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

            // WALL FOLLOW (PHYSICAL BUTTON Y)
            if (controller->x()) {
                    Serial.print("WALL FOLLOWING");
                while(1) {
                    BP32.update();
                    // loop code here
                    Wall_Follow();
                    if(controller->a() == 1) {
                        break;
                    }
                }
            }

            // COLOR DETECTION (PHYSICAL BUTTON X probably)
            if (controller->y()) {
                    Serial.print("COLOR SAMPLING");
                while(1) {
                    BP32.update();
                    color_challenge();
                   // if(controller->a() == 1) {
                    //    break;
                    break;
                    // setup and loop code here
                   // }
                }
            }
    }
        vTaskDelay(1);
    }
}