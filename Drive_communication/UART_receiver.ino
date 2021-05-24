const byte numChars = 32;
char receivedChars[numChars];
boolean newData = false;

void setup() {
    Serial.begin(9600);
    //Serial.println("<Arduino is ready>");
}

void loop() {
    receive_data();
    showNewData();
}

void receive_data() {
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

void showNewData() {
    if (newData == true) {
        Serial.print("This just in ... ");
        Serial.println(receivedChars);
        newData = false;
    }
}