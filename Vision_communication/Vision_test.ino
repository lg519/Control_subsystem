#include <SPI.h>

//SPI settings
int ss = 14;
SPISettings settingsA(2000000, LSBFIRST, SPI_MODE1);

//Vision_communication setup
char receivedData_Vision[255];

void setup()
{
    //Debug communication UART port
    Serial.begin(115200);

    //start the SPI library:
    pinMode(ss, OUTPUT);
    SPI.begin();

}
void loop() 
{
    receive_data_Vision();
}

void receive_bytes_Vision()
{
    static bool recvInProgress_Vision = false;
    static byte counter_Vision = 0;
    char SOM = '{';
    char EOM = 99;
    char received_byte;
    int counter_error = 1;
        
    while (true)
    {
        received_byte = SPI.transfer(0);
        
        //
        if(received_byte == 0) continue;
        //Serial.println((int)received_byte);
        //discard bytes until SOM is encounterd
        if (received_byte == SOM)
        {
            recvInProgress_Vision = true;
            continue;
        }

        //receive data
        if (recvInProgress_Vision == true)
        {
            if (received_byte == EOM)
            {
                recvInProgress_Vision = false;
                counter_Vision = 0;
                //Serial.println("end of data received from vision");
                return;
            }
            receivedData_Vision[counter_Vision] = received_byte;
            counter_Vision++;
            //Serial.println("counter_error is:");
            //Serial.print((int)counter_error);
            if(counter_error != received_byte) Serial.println("ERROOOOOOOOOOOOOOOOR!");
            counter_error++;
        }
    }
}

void receive_data_Vision()
{
    SPI.beginTransaction(settingsA);

    digitalWrite(ss, LOW);
    receive_bytes_Vision();
    digitalWrite(ss, HIGH);

    SPI.endTransaction();
}
