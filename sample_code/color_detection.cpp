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
    //This counter is just to keep track of time. 
    int counter = 0;
    //this boolean says if we have found the final color or not. 
    //Once it's set to true, the while loop will end and so will the function. 
    bool final_color_found = false;

    //this bool says if we have found any new colors besides the original one 
    bool new_color_found = false;

    //this guy holds whatever the sample color is
    char sample_color == 'z';

    // these guys are helper variables for the color_detector function
    int r, g, b, a;

    //this guy holds whatever color we are on rn. 
    char last_color_detected = 'r';
    
    //this guy helps with the part of the code that flashes a certain number of times 
    //to signal what color the sample was
    bool flash_has_happened = false;

    //This thing needs to run right at the beginning it's to initialize the color or something idk
    while (!apds.colorAvailable()) {
        delay(5);
    }

    while (final_color_found == false) {
        
        //these two lines help with the function that determines what color we are currently on.
        apds.readColor(r, g, b, a); //assigns color ints a value based on sensor
        last_color_detected = color_detector(r,g,b,a); //takes in color ints and returns whatever the strongest one is

        //This if statement will run a few times right at the beginning of our program
        //it's purpose is to identify what the sample color that we need to store is.
        //this statement should not be running at all after the first couple seconds
        if ((counter>50) and (counter<90))
        {
            sample_color = last_color_detected;
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
                    Serial.println("SAMPLE COLOR FINAL IS RED")

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
                    Serial.println("SAMPLE COLOR FINAL IS GREEN")
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


            if ((last_color_detected == sample_color) && (new_color_found == true)) {

                Serial.println("I HAVE FOUND A COLOR MATCHING THE SAMPLE COLOR");
                final_color_found = true; 
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
