#include <ArduinoJson.h>
#include <stdint.h>
#include <WiFi.h>
#include <HTTPClient.h>


//Drive_communication setup
const byte numChars_Drive = 3 ;
char receivedChars_Drive[numChars_Drive];
bool newData_Drive = false;


//Server_communication setup
const char* ssid = "DESKTOP-SIM2NC1 1364";
const char* password = "o9O)5498";
String serverName = "https://rover-flask-server-6w7fs.ondigitalocean.app/rover_test";


void setup() {

    //Debug communication UART port
    Serial.begin(115200);

    //Drive_communication UART port
    Serial2.begin(9600, SERIAL_8N1, 16, 17);

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


    StaticJsonDocument<96> JSON_Drive_to_Control;
    StaticJsonDocument<32> JSON_Control_to_Drive;

    receive_data_Drive();
    update_JSON_obj_Drive(JSON_Drive_to_Control); 

    POST_data_Server(JSON_Drive_to_Control, JSON_Control_to_Drive);

    send_data_Drive(JSON_Control_to_Drive);

}



void receive_data_Drive() {
    static bool recvInProgress_Drive = false;
    static byte index_Drive = 0;
    char startMarker = '{';
    char endMarker = '}';
    char incoming_byte;

    
 
    while (Serial2.available() > 0 && newData_Drive == false) {

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
            }
        }
        else if (incoming_byte == startMarker) 
        {
            recvInProgress_Drive = true;
        }
    }
}


void update_JSON_obj_Drive(JsonDocument& JSON_Drive_to_Control) {
    if (newData_Drive == true) {
        
        uint8_t x_coordinate = receivedChars_Drive[2];
        uint8_t y_coordinate = receivedChars_Drive[1];
        uint8_t theta = receivedChars_Drive[0];


        //Serial.println("data received from Drive");
        //Serial.print("movement_id is: ");
        //Serial.println(movement_id);
        //Serial.print("delta_x is: ");
        //Serial.println(delta_x);
        //Serial.print("delta_y is: ");
        //Serial.println(delta_y);
        //Serial.print("delta_theta is: ");
        //Serial.println(delta_theta);
        //Serial.print("speed_ is: ");
        //Serial.println(speed_);


        JSON_Drive_to_Control["x_coordinate"] = x_coordinate;
        JSON_Drive_to_Control["y_coordinate"] = y_coordinate;
        JSON_Drive_to_Control["theta"] = theta;

        newData_Drive = false;
    }
}

void POST_data_Server(JsonDocument& JSON_Drive_to_Control, JsonDocument& JSON_Control_to_Drive) {
    
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
        serializeJson(JSON_Drive_to_Control, json);
        int httpResponseCode = http.POST(json);

        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);


        // Read response 
        Serial.print(http.getString());

        DeserializationError error = deserializeJson(JSON_Control_to_Drive, http.getStream());

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

void send_data_Drive(JsonDocument& JSON_Control_to_Drive) 
{
    if (!JSON_Control_to_Drive.isNull()) 
    {
        //extract values from JSON document
        uint8_t x_coordinate = JSON_Control_to_Drive["x_coordinate"]; 
        uint8_t y_coordinate = JSON_Control_to_Drive["y_coordinate"];

        Serial2.write('{');
        Serial2.write(x_coordinate); 
        Serial2.write(y_coordinate);
        Serial2.write('}');
    }
    else
    {
        Serial.println("error: JSON_Control_to_Drive is Null");
    }
}