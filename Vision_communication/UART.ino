#include <SPI.h>


//Vision_communication setup
char receivedData_Vision[255];

void setup()
{
    //Debug communication UART port
    Serial.begin(115200);

    Serial2.begin(9600, SERIAL_8N1, 16, 17);


}
void loop() 
{
    receive_bytes_Vision();
}

void receive_bytes_Vision()
{
    static bool recvInProgress_Vision = false;
    static byte counter_Vision = 0;
    char SOM = '{';
    char EOM = '}';
    char received_byte;
    int counter_error = 1;
        
    while (true)
    {
        while (Serial2.available() > 0){
            received_byte = Serial2.read();
            
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
}


