#include <stdint.h>




//Control_communication setup
const byte numChars_Control = 3;
char receivedChars_Control[numChars_Control];
bool newData_Control = false;


void setup() {
    Serial.begin(115200);
    Serial1.begin(9600);
}

void loop() {

    send_data_Control();
    delay(5000);
    //receive_data_Control();
    //print_data_Control();
  
}

void send_data_Control() {

    uint8_t movement_id = 12;
    uint8_t delta_x = 3;
    uint8_t delta_y = 7;
    uint8_t delta_theta = 9;
    uint8_t speed_ = 11;


    Serial.println("data sent to control")
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


    Serial1.write('{');
    Serial1.write(movement_id);
    Serial1.write(delta_x); 
    Serial1.write(delta_y);
    Serial1.write(delta_theta);
    Serial1.write(speed_)
    Serial1.write('}');
}

void receive_data_Control() {

    static boolean recvInProgress_Control = false;
    static byte index_Control = 0;
    char startMarker = '{';
    char endMarker = '}';
    char incoming_byte;

    while (Serial1.available() > 0 && newData_Control == false) {

        incoming_byte = Serial1.read();

        if (recvInProgress_Control == true) {
            if (incoming_byte != endMarker) {
                receivedChars_Control[index_Control] = incoming_byte;
                index_Control++;
                //if (index_Control >= numChars_Control) {
                //    index_Control = numChars_Control - 1;
                //}
            }
            else {
                //receivedChars_Control[index_Control] = '\0'; // terminate the string
                recvInProgress_Control = false;
                index_Control = 0;
                newData_Control = true;
            }
        }

        else if (incoming_byte == startMarker) {
            recvInProgress_Control = true;
        }
    }
}

void print_data_Control() {
    if (newData_Control == true) {
        
        uint8_t movement_id  = receivedChars_Control[2];
        uint8_t delta_x = receivedChars_Control[1];
        uint8_t delta_y = receivedChars_Control[0];

        Serial.println("data received from Control");
        Serial.print("movement_id is: ");
        Serial.println(movement_id);
        Serial.print("delta_x is: ");
        Serial.println(delta_x);
        Serial.print("delta_y is: ");
        Serial.println(delta_y);
        
        newData_Control = false;
    }
}