void setup()  {
    Serial.begin(115200);
    front.setFilterRate(0.5f);
    left.setFilterRate(0.5f);
    right.setFilterRate(0.5f);
    servo1.attach(15);
    servo2.attach(2);
}

void loop() {
    Serial.print("Front Sensor: ");
    Serial.println(front.getDistanceFloat());
    Serial.print("Left Sensor: "); 
    Serial.println(left.getDistanceFloat());
    Serial.print("Right Sensor: ");
    Serial.println(right.getDistanceFloat());

    if(front.getDistanceFloat() >= 10.00){
        Serial.println("Continue Straight");
        servo1.writeMicroseconds(1000);
        servo2.writeMicroseconds(2000);
    }
    else if((front.getDistanceFloat() < 10.00)&&(right.getDistanceFloat() >= 10.00)&&(left.getDistanceFloat() < 10.00)) {
        Serial.println("Turn Right");
        servo1.writeMicroseconds(1000);
        servo2.writeMicroseconds(1000);
    }   
    else if((front.getDistanceFloat() < 10.00) && (left.getDistanceFloat() >= 10.00) && (right.getDistanceFloat() >= 10.00)) {
        Serial.println("Turn Left");
        servo1.writeMicroseconds(2000);
        servo2.writeMicroseconds(2000);
    }
    else{
        Serial.println("Stop");
        servo1.writeMicroseconds(1500);
        servo2.writeMicroseconds(1500);
    }
} 