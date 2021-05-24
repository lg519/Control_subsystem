#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>


//Server_communication setup
const char* ssid = "DESKTOP-SIM2NC1 1364";
const char* password = "o9O)5498";
String serverName = "https://rover-flask-server-6w7fs.ondigitalocean.app/rover_test";

unsigned long lastTime = 0;
unsigned long timerDelay = 5000;



//Drive_communication setup
const byte numChars = 6;
char receivedChars[numChars];
boolean newData = false;


void setup() {

    //Debug communication UART port
    Serial.begin(9600);

    //Drive_communication UART port
    Serial1.begin(9600);

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

    StaticJsonDocument<384> doc;
    
    receive_data_Drive();
    update_JSON_obj_Drive();  

    receive_data_Vision();
    update_JSON_obj_Vision();

    POST_data_Server();

    send_data_Drive();

}



void receive_data_Drive() {
    static bool recvInProgress_Drive = false;
    static byte index_Drive = 0;
    char startMarker = '{';
    char endMarker = '}';
    char incoming_byte;
 
    while (Serial2.available() > 0 && newData_Drive == false) {

        incoming_byte = Serial2.read();

        if (recvInProgress_Drive == true) {
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

        else if (incoming_byte == startMarker) {
            recvInProgress_Drive = true;
        }
    }
}








void receive_data_Drive() {
    static boolean recvInProgress = false;
    static byte index = 0;
    char startMarker = '{';
    char endMarker = '}';
    char incoming_byte;
 
    while (Serial.available() > 0 && newData == false) {

        incoming_byte = Serial.read();

        if (recvInProgress == true) {
            if (incoming_byte != endMarker) {
                receivedChars[index] = incoming_byte;
                index++;
                //if (index >= numChars) {
                //    index = numChars - 1;
                //}
            }
            else {
                receivedChars[index] = '\0'; // terminate the string
                recvInProgress = false;
                index = 0;
                newData = true;
            }
        }

        else if (incoming_byte == startMarker) {
            recvInProgress = true;
        }
    }
}


void update_JSON_obj_Drive() {
    if (newData == true) {
        //Serial.print("This just in ... ");
        //Serial.println(receivedChars);
        newData = false;
    }
}

void send_data_Drive() {
    //TODO
}


void POST_data_Server() {
    if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){

        HTTPClient http;
        String serverPath = serverName;
        
        //create JSON object
        // ...
        // ...
        // ...

        http.useHTTP10(true);
        // Domain name with URL path or IP address with path
        http.begin(serverPath.c_str()); 
        http.addHeader("Content-Type", "application/json");
    
        // Send HTTP POST request
        int httpResponseCode = http.POST(json);

        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);

        // Read response
        Serial.print(http.getString());

        // Free resources
        http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}



void receive_data_Vision() {
    //TODO
}

void update_JSON_obj_Vision() {
    //TODO
}