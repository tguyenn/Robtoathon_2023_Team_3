#include "RASLib/inc/common.h"
#include "RASLib/inc/adc.h"

static tADC *IRSensor;
const tPin out = PIN_A0;

int main() {
    IRSensor InitializeADC(out);
    while (!KeyWasPressed()) {

        Printf("IR Values: %1.3f\r", ADCRead(IRSensor));
    }
    return 0; 

}