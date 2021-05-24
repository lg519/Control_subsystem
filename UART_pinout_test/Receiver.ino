char receivedChar;
boolean newData = false;

void setup() {
    Serial.begin(9600);
    Serial.println("<Arduino is ready>");

    //receive data from arduino
    Serial2.begin(9600, SERIAL_8N1, 16, 17);
}

void loop() {
    recvOneChar();
    showNewData();
}

void recvOneChar() {
    if (Serial2.available() > 0) {
    receivedChar = Serial2.read();
    newData = true;
    }
}

void showNewData() {
    if (newData == true) {
    Serial.print("This just in ... ");
    Serial.println(receivedChar);
    newData = false;
    }
}