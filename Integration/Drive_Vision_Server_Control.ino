#include <ArduinoJson.h>
#include <stdint.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>


//Drive_communication setup
char buffer_Drive[3];

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

    receive_data_Drive();
    update_JSON_obj_Drive(JSON_Drive_to_Server); 

    receive_data_Vision();
    update_JSON_obj_Vision(JSON_Drive_to_Server);

    POST_data_Server(JSON_Drive_to_Server, JSON_Server_to_Drive);

    send_data_Drive(JSON_Server_to_Drive);
}

void receive_data_Drive() {
    static bool receiveData_Drive = false;
    static byte index_Drive = 0;
    
    char startMarker = '{';
    char endMarker = '}';
    char incoming_byte;

    
    while(true)
    {
        while (Serial2.available() > 0 && newData_Drive == false) 
        {
            incoming_byte = Serial2.read();
            
            //Rewrite this code

            if (receiveData_Drive == true) 
            {
                if (incoming_byte != endMarker) {
                    receivedChars_Drive[index_Drive] = incoming_byte;
                    index_Drive++;
                }
                else {
                    receiveData_Drive = false;
                    index_Drive = 0;
                    return;
                }
            }
            else if (incoming_byte == startMarker) 
            {
                receiveData_Drive = true;
            }
        }
    }
}

void update_JSON_obj_Drive(JsonDocument& JSON_Drive_to_Server) 
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
    


    JSON_Drive_to_Server["delta_x"] = delta_x;
    JSON_Drive_to_Server["delta_y"] = delta_y;
    JSON_Drive_to_Server["delta_theta"] = delta_theta;

    newData_Drive = false;
    
}