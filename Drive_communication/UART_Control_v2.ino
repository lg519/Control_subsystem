#include <ArduinoJson.h>
#include <stdint.h>


//Drive_communication setup
const byte numChars_Drive = 5;
char receivedChars_Drive[numChars_Drive];
bool newData_Drive = false;


void setup() {

    //Debug communication UART port
    Serial.begin(115200);

    //Drive_communication UART port
    Serial2.begin(9600, SERIAL_8N1, 16, 17);

}

void loop() {

    StaticJsonDocument<384> doc;

    receive_data_Drive();
    update_JSON_obj_Drive(doc);

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


void update_JSON_obj_Drive(JsonDocument& doc) {
    if (newData_Drive == true) {
        
        uint8_t movement_id  = receivedChars_Drive[4];
        uint8_t delta_x = receivedChars_Drive[3];
        uint8_t delta_y = receivedChars_Drive[2];
        uint8_t delta_theta = receivedChars_Drive[1];
        uint8_t speed_ = receivedChars_Drive[0];


        Serial.println("data received from Drive");
        Serial.print("movement_id is: ");
        Serial.println(movement_id);
        Serial.print("delta_x is: ");
        Serial.println(delta_x);
        Serial.print("delta_y is: ");
        Serial.println(delta_y);
        Serial.print("delta_theta is: ");
        Serial.println(delta_theta);
        Serial.print("speed_ is: ");
        Serial.println(speed_);


        doc["movement_id"] = movement_id;
        doc["delta_x"] = delta_x;
        doc["delta_y"] = delta_y;
        doc["delta_theta"] = delta_theta;
        doc["speed"] = speed_;
        

        newData_Drive = false;
    }
}


void send_data_Drive() {

    uint8_t movement_id = 18;
    uint8_t delta_x = 1;
    uint8_t delta_y = 2;

    Serial2.write('{');
    Serial2.write(movement_id);
    Serial2.write(delta_x); 
    Serial2.write(delta_y);
    Serial2.write('}');
}