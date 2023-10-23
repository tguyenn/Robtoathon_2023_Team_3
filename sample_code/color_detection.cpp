//headers
#include <Wire.h>
#include <Arduino_APDS9960.h>
#include <bits/stdc++.h>

//color sensor definitions
#define APDS9960_INT 2
#define I2C_SDA 21
#define I2C_SCL 22
#define I2C_FREQ 100000

//color sensor unit & I2C unit
TwoWire I2C_0 = TwoWire(0);
APDS9960 apds = APDS9960(I2C_0, APDS9960_INT);

void setup() {
    //sets up I2C protocol
    I2C_0.begin(I2C_SDA, I2C_SCL, I2C_FREQ);

    //sets up color sensor
    apds.setInterruptPin(APDS9960_INT);
    apds.begin();
    Serial.begin(115200);
}




char last_color_detected = 'r';

char return_strongest_color_detected(int r, int g, int b, int a) {
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


// todo: 
//      store greatest color int in int goalColor
//      needs findButtonColor method
//      if (r > g), store, etc.


void loop() {
    int r, g, b, a;
    int upper, lower;
    upper = 20;
    lower = 10; //adjust based on testing

    //wait until color is read from sensor
    while (!apds.colorAvailable()) {
        delay(5);
    }

    //read color from sensor
    apds.readColor(r, g, b, a); //assigns color ints a value based on sensor

    //print color in decimal
    Serial.print("RED: ");
    Serial.println(r);
    Serial.print("GREEN: ");
    Serial.println(g);
    Serial.print("BLUE: ");
    Serial.println(b);
    Serial.print("AMBIENT(IGNORE): ");
    Serial.println(a);

    last_color_detected = return_strongest_color_detected(r,g,b,a);

    Serial.println("last color detected is ");
    Serial.print(last_color_detected);
    Serial.println(" ");

}
