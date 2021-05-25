#include <SPI.h>

int ss = 14;
SPISettings settingsA(2000000, LSBFIRST, SPI_MODE1);

//Vision_communication setup
const byte numChars_Vision = 3*5;  //recive up to 5 triplets { BALL, ANGLE, DEPTH; BALL, ANGLE, DEPTH; ...}
char receivedChars_Vision[numChars_Vision];
bool newData_Vision = false;


void setup() 
{
    //Debug communication UART port
    Serial.begin(115200);
    
    //start the SPI library:
    pinMode(ss, INPUT); 
    SPI.begin();
}

void loop() 
{
    recive_data_Vision();   
}


void recive_bytes_Vision()
{
    static bool recvInProgress_Vision = false;
    static byte index_Vision = 0;
    char startMarker = '{';
    char endMarker = '}';
    char incoming_byte;

    while (newData_Vision == false) {

        incoming_byte =  SPI.transfer(0);

        if (recvInProgress_Vision == true) 
        {
            if (incoming_byte != endMarker) {
                receivedChars_Vision[index_Vision] = incoming_byte;
                index_Vision++;
            }
            else {
                recvInProgress_Vision = false;
                index_Vision = 0;
                newData_Vision = true;
            }
        }
        else if (incoming_byte == startMarker) 
        {
            recvInProgress_Vision = true;
        }
    }
}

void receive_data_Vision()
{
    SPI.beginTransaction(settingsA);
 
    digitalWrite(ss,LOW);
    recive_bytes_Vision();
    digitalWrite(ss,HIGH);

    SPI.endTransaction();
}