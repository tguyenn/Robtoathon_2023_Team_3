

// ONLY PLUG IR SENSOR INTO 3.3V POWER SUPPLY
// RED IS GROUND
// BLACK IS POWER (lol)

#include <ESP32SharpIR.h>

ESP32SharpIR left(ESP32SharpIR::GP2Y0A21YK0F, 36);

void setup()  {
// setiflterrate -> sensor gathers data in bursts... getdistancefloat gives average of bursts
    left.setFilterRate(0.1f);
}

void loop() {
    Serial.println(left.getDistanceFloat());
    delay(500);
}