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

void lineSetup() {
// set up Serial Communication and sensor pins 
// Serial.begin(115200);

// set thresholds easily
public int lower, upper;
lower = 600; 
upper = 900;   

qtr.setTypeRC(); // or setTypeAnalog() 
qtr.setSensorPins ((const uint8_t []) {12, 14, 27, 26}, 4);

servo1.attach(15);
servo2.attach(2);
// calibration sequence
    for (uint8_t i = 0; i < 250; i++) {
        Serial.println("calibrating"); 
        qtr.calibrate();
        delay(20);
    }
 

}


void lineLoop() {
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
}