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
QTRSensors qtr;

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
    uint16_t sensors[4];
    int16_t position = qtr.readLineBlack(sensors);
    
    Serial.print("sensor1: ");
    Serial.println(sensors[0]);
    Serial.print("sensor2: ");
    Serial.println(sensors[1]);
    Serial.print("sensor3: ");
    Serial.println(sensors[2]);
    Serial.print("sensor4: ");
    Serial.println(sensors[3]);

    delay(100);


    // 900+ means LINE 
    // 300 or less means NO LINE
    // I represents line, o represents no line
    
    // easily change lower threshold 
    int lower, upper;
    lower = 600; 
    upper = 900;    

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

    // error test
    // if(sensors[0] > 900 && sensors[1] > 900 && sensors[2] > 900 && sensors[3] > 900) {
    //     Serial.println("error");
    // }

    // oooo
    // sweep clockwise ONLY 90 degrees and then sweep counterclockwise to make sure no backtracking happens

}