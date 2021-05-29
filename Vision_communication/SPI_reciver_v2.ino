#include <SPI.h>
#include <ArduinoJson.h>
#include <stdint.h>

//SPI settings
int ss = 14;
SPISettings settingsA(2000000, LSBFIRST, SPI_MODE1);

//Vision_communication setup
char receivedChars_Vision[3 * 5]; //recive up to 5 triplets { BALL, ANGLE, DEPTH; BALL, ANGLE, DEPTH; ...}

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
    StaticJsonDocument<512> JSON_Vision_to_Server;
    receive_data_Vision();
    update_JSON_obj_Vision(JSON_Vision_to_Server);
    delay(1000);
}

void receive_bytes_Vision()
{
    //Vision_communication epilogue
    static bool newData_Vision = false;
    static bool recvInProgress_Vision = false;
    static byte index_Vision = 0;
    static unsigned int triplets_received = 0;

    char startMarker = '{';
    char endMarker = '}';
    char incoming_byte;

    while (triplets_received != 5)
    {
        //receive one byte
        incoming_byte = SPI.transfer(0);
        //Serial.print("byte received is: ");
        //Serial.println((int)incoming_byte);
        if (incoming_byte == startMarker)
            recvInProgress_Vision = true;
        else if (recvInProgress_Vision == true)
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
            else
            {
                recvInProgress_Vision = false;
                newData_Vision = true;
                triplets_received++;
            }
        }
    }
    //Vision_communication prologue
    index_Vision = 0;
    triplets_received = 0;
}

void receive_data_Vision()
{
    SPI.beginTransaction(settingsA);

    digitalWrite(ss, LOW);
    receive_bytes_Vision();
    digitalWrite(ss, HIGH);

    SPI.endTransaction();
}

void update_JSON_obj_Vision(JsonDocument &JSON_Vision_to_Server)
{
    JsonArray Vision_data = JSON_Vision_to_Server.createNestedArray("Vision_data");

    for (int i = 0; i < 3*5; i += 3)
    {
        JsonObject triplet = Vision_data.createNestedObject();
        triplet["colour"] = receivedChars_Vision[i];
        triplet["x_min"] = receivedChars_Vision[i + 1];
        triplet["x_max"] = receivedChars_Vision[i + 2];
    }

    Serial.println("data received from Vision: ");

    for (int i = 0; i < 15 ; i++)
    {
        Serial.println(receivedChars_Vision[i]);
    }
    
    for (int i = 0; i < 15 ; i++)
    {
      receivedChars_Vision[i]=0;
    }

    // for (JsonObject elem : JSON_Vision_to_Server["Vision_data"].as<JsonArray>())
    // {
    //     uint8_t coulor = elem["coulor"];
    //     uint8_t x_min = elem["x_min"];
    //     uint8_t x_max = elem["x_max"];
    //     Serial.println(coulor);
    //     Serial.println(x_min);
    //     Serial.println(x_max);
    // }
}