#include <WiFi.h>
#include <WebSocketClient.h>
#include <ArduinoJson.h>
 
const char *ssid = "max";
const char *password = "12345678";
 
char path[] = "/rover_data";
char host[] = "10.42.0.1:5000";
 
WebSocketClient webSocket;
WiFiClient client;

void setup() {
  Serial.begin(115200);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  webSocket.begin("10.42.0.1",5000,"/");
  webSocket.onEvent(webSocketEvent);
  webSocket.reconnectInterval(5000);

  
 
  delay(5000);
 
  if (client.connect(host, 5000)) {
    Serial.println("Connected");
  } else {
    Serial.println("Connection failed.");
  }
 
  webSocketClient.path = path;
  webSocketClient.host = host;
  if (webSocketClient.handshake(client)) {
    Serial.println("Handshake successful");
  } else {
    Serial.println("Handshake failed.");
  }
 
}