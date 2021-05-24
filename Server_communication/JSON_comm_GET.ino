#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "DESKTOP-SIM2NC1 1364";
const char* password = "o9O)5498";

//Your Domain name with URL path or IP address with path
String serverName = "http://arduinojson.org/example.json";


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
      

      http.useHTTP10(true);
      // Domain name with URL path or IP address with path
      http.begin(serverPath.c_str());     
      // Send HTTP GET request
      int httpResponseCode = http.GET();

      
      if (httpResponseCode>0) {
        //Serial.print("HTTP Response code: ");
        //Serial.println(httpResponseCode);
        
        //String payload = http.getString();
        //Serial.println(payload);

        // Parse response
        StaticJsonDocument<128> doc;

        DeserializationError error = deserializeJson(doc, http.getStream());
        
        if (error) {
          Serial.print(F("deserializeJson() failed: "));
          Serial.println(error.f_str());
          return;
        }
        
        const char* sensor = doc["sensor"]; // "gps"
        long time = doc["time"]; // 1351824120
        
        float data_0 = doc["data"][0]; // 48.75608
        float data_1 = doc["data"][1]; // 2.302038

        Serial.println(sensor);

        
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}