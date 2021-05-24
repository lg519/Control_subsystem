void setup() {
    Serial.begin(9600);

    //communication UART port
    Serial1.begin(9600);

}


void loop() {

    Serial.print("data sent is: ");
    Serial.println('{');
    Serial1.write('{');
    delay(1000);  

}