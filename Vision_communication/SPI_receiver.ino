#include <SPI.h>

int ss = 14;
SPISettings settingsA(2000000, LSBFIRST, SPI_MODE1);


void setup() {
    //Debug communication UART port
    Serial.begin(115200);
    
    // start the SPI library:
    pinMode(ss, INPUT); 
    SPI.begin();
}

void loop() {}