#include <ArduinoJson.h>
#include <stdint.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>

//Drive_communication setup
char receivedData_Drive[3];

//Server_communication setup
const char* ssid = "DESKTOP-SIM2NC1 1364";
const char* password = "o9O)5498";
String serverName = "https://rover-flask-server-6w7fs.ondigitalocean.app/api/rover_data";

//SPI settings
int ss = 14;
SPISettings settingsA(2000000, LSBFIRST, SPI_MODE1);

//Vision_communication setup
char receivedData_Vision[3 * 5 + 80]; //recive 5 triplets and 80 bytes array { colour, x_min, x_max; + ...

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

    StaticJsonDocument<608> JSON_Rover_to_Server;
    StaticJsonDocument<96> JSON_Server_to_Rover;

    receive_data_Drive();
    update_JSON_obj_Drive(JSON_Rover_to_Server); 

    receive_data_Vision();
    update_JSON_obj_Vision(JSON_Rover_to_Server);

    POST_data_Server(JSON_Rover_to_Server, JSON_Server_to_Rover);

    send_data_Drive(JSON_Server_to_Rover);
}

void receive_data_Drive() {
    static bool recvInProgress_Drive = false;
    static byte counter_Drive = 0;
    char SOM = '{';
    char EOM = '}';
    char received_byte;

    while(true)
    {
        while (Serial2.available() > 0) 
        {
            received_byte = Serial2.read();
            //discard bytes until SOM is encounterd
            if (received_byte == SOM) 
            {
                recvInProgress_Drive = true;
                continue;
            }

            //receive data
            if (recvInProgress_Drive == true) 
            {
                if (received_byte == EOM)
                {
                    recvInProgress_Drive = false;
                    counter_Drive = 0;
                    return;
                }
                receivedData_Drive[counter_Drive] = received_byte;
                counter_Drive++;
            }
        }
    }
}

void update_JSON_obj_Drive(JsonDocument& JSON_Rover_to_Server) 
{        
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
    


    JSON_Rover_to_Server["delta_x"] = delta_x;
    JSON_Rover_to_Server["delta_y"] = delta_y;
    JSON_Rover_to_Server["delta_theta"] = delta_theta;
 
}

void receive_bytes_Vision()
{
    static bool recvInProgress_Vision = false;
    static byte counter_Vision = 0;
    char SOM = '{';
    char EOM = '}';
    char received_byte;

    while(true)
    {
        while (Serial2.available() > 0) 
        {
            incoming_byte = SPI.transfer(0);
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
                    return;
                }
                receivedData_Vision[counter_Vision] = received_byte;
                counter_Vision++;
            }
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

void update_JSON_obj_Vision(JsonDocument &JSON_Rover_to_Server)
{
    JsonArray Vision_data = JSON_Rover_to_Server.createNestedArray("balls");

    for (int i = 0; i < 3*5; i += 3)
    {
        JsonObject triplet = Vision_data.createNestedObject();
        triplet["colour"] = "red";
        triplet["theta"] = receivedChars_Vision[i + 1];
        triplet["depth"] = receivedChars_Vision[i + 2];
    }

    JsonArray depth = JSON_Rover_to_Server.createNestedArray("depth");
    
    for (int i = 15; i < 95; i++)
    {
        depth.add(receivedChars_Vision[i]);
    }

}

void POST_data_Server(JsonDocument& JSON_Rover_to_Server, JsonDocument& JSON_Server_to_Rover) {
    
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