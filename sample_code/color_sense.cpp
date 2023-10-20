#include "sdkconfig.h"
#ifndef CONFIG_BLUEPAD32_PLATFORM_ARDUINO
#error "Must only be compiled when using Bluepad32 Arduino platform"
#endif  // !CONFIG_BLUEPAD32_PLATFORM_ARDUINO

#include <Arduino.h>
#include <Wire.h>
#include <Arduino_APDS9960.h>
#include <bits/stdc++.h>











// IMPORTANT PLEASE READ
// ONLY PLUG COLOR SENSOR INTO 3.3V
// MUST use pins 21 (SDA) and 22 (SCL)
//
//
//
//
//
//
// READ^^^^^^(please☹️🙏 dont burn the sensor like i did with the IR sensor)

// color sensor definitions
#define APDS9960_INT 2
#define I2C_SDA 21
#define I2C_SCL 22
#define I2C_FREQ 100000

// color sensor shouldnt be too close to the ground because too much darkness will give bad readings
TwoWire I2C_0 = TwoWire(0);
APDS9960 apds = APDS9960(I2C_0, APDS996_INT);

void setup() {
    // set up I2C protocol
    I2C_0.begin(I2C_SDA, I2C_SCL, I2C_FREQ);

    // set up color sensor
    apds.setInterruptPin(APDS9960_INT);
    apds.begin();
    Serial.begin(115200);

}

void loop() {
    
    int r, g, b, a;

    // wait until color is read from sensor
    while (!apds.colorAvailable()) {
        delay(5);
    }

    // read color from sensor
    apds.readColor(r, g, b, a);


    Serial.print("R: ");
    Serial.println(r);
    Serial.print("G: ");
    Serial.println(g);
    Serial.print("B: ");
    Serial.println(b);
    Serial.print("AMBIENT: "); // ambient lighting... might be useful in determining reason for bad readings
    Serial.println(a);
}