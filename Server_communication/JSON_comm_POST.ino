#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "DESKTOP-SIM2NC1 1364";
const char* password = "o9O)5498";

//Your Domain name with URL path or IP address with path
String serverName = "https://rover-flask-server-6w7fs.ondigitalocean.app/rover_test";


unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;

void setup() {
    Serial.begin(115200); 

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
  
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){

        HTTPClient http;

        String serverPath = serverName;
        

        //create JSON object
        StaticJsonDocument<768> doc;

        JsonArray depth = doc.createNestedArray("depth");
        depth.add(0);
        depth.add(0);
        depth.add(0);
        depth.add(0);
        depth.add(0);
        depth.add(0);
        depth.add(0);
        depth.add(0);
        depth.add(0);
        depth.add(0);
        depth.add(0);
        depth.add(0);
        depth.add(0);
        depth.add(0);
        depth.add(0);

        JsonArray balls = doc.createNestedArray("balls");

        JsonObject balls_0 = balls.createNestedObject();
        balls_0["colour"] = "red";
        balls_0["theta"] = 5;
        balls_0["depth"] = 7;

        JsonObject balls_1 = balls.createNestedObject();
        balls_1["colour"] = "blue";
        balls_1["theta"] = 8;
        balls_1["depth"] = 4;
        doc["delta_x"] = 2;
        doc["delta_y"] = 3;
        doc["delta_theta"] = 6;
        doc["speed"] = 7;
        doc["movement_id"] = 12423;
        doc["battery_charge"] = 23;

        String json;
        serializeJson(doc, json);

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