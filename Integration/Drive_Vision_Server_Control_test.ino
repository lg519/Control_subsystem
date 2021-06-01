#include <ArduinoJson.h>
#include <stdint.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>


//Drive_communication setup
const byte numChars_Drive = 3 ;
char receivedChars_Drive[numChars_Drive];
bool newData_Drive = false;

//Server_communication setup
const char* ssid = "DESKTOP-SIM2NC1 1364";
const char* password = "o9O)5498";
String serverName = "https://rover-flask-server-6w7fs.ondigitalocean.app/api/rover_data";

//SPI settings
int ss = 14;
SPISettings settingsA(2000000, LSBFIRST, SPI_MODE1);

//Vision_communication setup
char receivedChars_Vision[3 * 5]; //recive up to 5 triplets { colour, x_min, x_max; colour, x_min, x_max; ...}

void setup() {

    //Debug communication UART port
    Serial.begin(115200);

    //Drive_communication UART port
    Serial2.begin(9600, SERIAL_8N1, 16, 17);

    //start the SPI library:
    pinMode(ss, OUTPUT);
    SPI.begin();

    //Server_communication WiFi connection
    WiFi.begin(ssid, password);
    Serial.println("Connecting");

    while(WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
}

void loop() {


    StaticJsonDocument<608> JSON_Drive_to_Server;
    StaticJsonDocument<96> JSON_Server_to_Drive;

    //receive_data_Drive();
    update_JSON_obj_Drive(JSON_Drive_to_Server); 

    receive_data_Vision();
    update_JSON_obj_Vision(JSON_Drive_to_Server);

    POST_data_Server(JSON_Drive_to_Server, JSON_Server_to_Drive);

    send_data_Drive(JSON_Server_to_Drive);

}

void receive_data_Drive() {
    static bool recvInProgress_Drive = false;
    static byte index_Drive = 0;
    char startMarker = '{';
    char endMarker = '}';
    char incoming_byte;

    
    while(true)
    {
        while (Serial2.available() > 0 && newData_Drive == false) 
        {
            incoming_byte = Serial2.read();

            if (recvInProgress_Drive == true) 
            {
                if (incoming_byte != endMarker) {
                    receivedChars_Drive[index_Drive] = incoming_byte;
                    index_Drive++;
                }
                else {
                    recvInProgress_Drive = false;
                    index_Drive = 0;
                    newData_Drive = true;
                    return;
                }
            }
            else if (incoming_byte == startMarker) 
            {
                recvInProgress_Drive = true;
            }
        }
    }
}

void update_JSON_obj_Drive(JsonDocument& JSON_Drive_to_Server) {
    //if (newData_Drive == true) {
        
        uint8_t delta_x = 0;
        uint8_t delta_y = 0;
        uint8_t delta_theta = 0;


        Serial.println("data received from Drive");
        Serial.print("delta_x is: ");
        Serial.println(delta_x);
        Serial.print("delta_y is: ");
        Serial.println(delta_y);
        Serial.print("delta_theta is: ");
        Serial.println(delta_theta);
        


        JSON_Drive_to_Server["delta_x"] = delta_x;
        JSON_Drive_to_Server["delta_y"] = delta_y;
        JSON_Drive_to_Server["delta_theta"] = delta_theta;

        newData_Drive = false;
    //}
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
    JsonArray Vision_data = JSON_Vision_to_Server.createNestedArray("balls");

    for (int i = 0; i < 3*5; i += 3)
    {
        if(receivedChars_Vision[i + 1]>= receivedChars_Vision[i + 2]) continue;
        JsonObject triplet = Vision_data.createNestedObject();
        triplet["colour"] = "red";
        triplet["theta"] = receivedChars_Vision[i + 1];
        triplet["depth"] = receivedChars_Vision[i + 2];
    }

    Serial.println("data received from Vision: ");

    // for (int i = 0; i < 15 ; i++)
    // {
    //     Serial.println(receivedChars_Vision[i]);
    // }

    // for (int i = 0; i < 15 ; i++)
    // {
    //   receivedChars_Vision[i]=0;
    // }

    for (JsonObject elem : JSON_Vision_to_Server["balls"].as<JsonArray>())
    {
        String colour = elem["colour"];
        uint8_t x_min = elem["theta"];
        uint8_t x_max = elem["depth"];
        Serial.println(colour);
        Serial.println((int)x_min);
        Serial.println((int)x_max);
    }
}

void POST_data_Server(JsonDocument& JSON_Drive_to_Server, JsonDocument& JSON_Server_to_Drive) {
    
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED)
    {
        HTTPClient http;
        String serverPath = serverName;

        http.useHTTP10(true);
        // Domain name with URL path or IP address with path
        http.begin(serverPath.c_str()); 
        http.addHeader("Content-Type", "application/json");
    
        // Send HTTP POST request
        String json;
        serializeJson(JSON_Drive_to_Server, json);
        int httpResponseCode = http.POST(json);

        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);


        // Read response 
        //Serial.print(http.getString());

        DeserializationError error = deserializeJson(JSON_Server_to_Drive, http.getStream());

        if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        }

        // Free resources
        http.end();
    }
    else
    {
        Serial.println("WiFi Disconnected");
    }   
}

void send_data_Drive(JsonDocument& JSON_Server_to_Drive) 
{
    if (!JSON_Server_to_Drive.isNull()) 
    {
        //extract values from JSON document
        uint8_t delta_x = JSON_Server_to_Drive["delta_x"]; 
        uint8_t delta_y = JSON_Server_to_Drive["delta_y"];

        Serial.println("data received from Server");
        Serial.print("delta_x is: ");
        Serial.println(delta_x);
        Serial.print("delta_y is: ");
        Serial.println(delta_y);
        

        Serial2.write('{');
        Serial2.write(delta_x); 
        Serial2.write(delta_y);
        Serial2.write('}');
    }
    else
    {
        Serial.println("error: JSON_Server_to_Drive is Null");
    }
}