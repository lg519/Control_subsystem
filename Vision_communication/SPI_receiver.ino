#include <SPI.h>
#include <ArduinoJson.h>

int ss = 14;
SPISettings settingsA(2000000, LSBFIRST, SPI_MODE1);

//Vision_communication setup
const byte numChars_Vision = 3*5;  //recive up to 5 triplets { BALL, ANGLE, DEPTH; BALL, ANGLE, DEPTH; ...}
char receivedChars_Vision[numChars_Vision];
bool newData_Vision = false;
byte index_Vision = 0;

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
    StaticJsonDocument<96> JSON_Vision_to_Server;
    receive_data_Vision();   
    update_JSON_obj_Vision(JSON_Vision_to_Server); 
}


void receive_bytes_Vision()
{
    Serial.println("receive_bytes_Vision");
    static bool recvInProgress_Vision = false;
    
    char startMarker = '{';
    char endMarker = '}';
    char incoming_byte;

    while(true){
        while (newData_Vision == false) {

            incoming_byte =  SPI.transfer(0);
            Serial.print("byte received: ");
            Serial.print(incoming_byte);

            if (recvInProgress_Vision == true) 
            {
                if (incoming_byte != endMarker) 
                {   
                    if (incoming_byte == 0)
                    {
                        //ignore incoming_byte 
                    }
                    else 
                    {
                        //append incoming_byte to array
                        receivedChars_Vision[index_Vision] = incoming_byte;
                        index_Vision++;
                    }
                }
                else {
                    Serial.println("END MARKER");
                    recvInProgress_Vision = false;
                    //index_Vision = 0;
                    newData_Vision = true;
                    return;
                }
            }
            else if (incoming_byte == startMarker) 
            {
                Serial.println("START MARKER");
                recvInProgress_Vision = true;
            }
        }
    }
}

void receive_data_Vision()
{
    Serial.println("receive_data_Vision");
    SPI.beginTransaction(settingsA);
 
    digitalWrite(ss,LOW);
    receive_bytes_Vision();
    digitalWrite(ss,HIGH);

    SPI.endTransaction();
}

void update_JSON_obj_Vision(JsonDocument& JSON_Vision_to_Server) 
{
    JsonArray Vision_data = JSON_Vision_to_Server.createNestedArray("Vision_data");

    for(int i=0; i<index_Vision; i+=3){
        JsonObject triplet = Vision_data.createNestedObject();
        triplet["colour"] = receivedChars_Vision[i];
        triplet["theta"] = receivedChars_Vision[i+1];
        triplet["depth"] = receivedChars_Vision[i+2];
    }
    
    
    
    Serial.println("data received from Drive");
    for (int i = 0; i <= index_Vision; i++)
    {
        Serial.println(receivedChars_Vision[i]);
    }
    
    index_Vision = 0;
}