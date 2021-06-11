#include <ArduinoJson.h>
#include <stdint.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>

//Drive_communication setup
char receivedData_Drive[5];

//Server_communication setup
//const char *ssid = "DESKTOP-SIM2NC1 1364";
//const char *password = "o9O)5498";
//String serverName = "https://rover-flask-server-6w7fs.ondigitalocean.app/api/rover_data";
const char *ssid = "max";
const char *password = "12345678";
String serverName = "http://10.42.0.1:6000/rover_data";


//SPI settings
int ss = 14;
SPISettings settingsA(2000000, LSBFIRST, SPI_MODE1);

//Vision_communication setup
char receivedData_Vision[3 * 5 + 80]; //recive 5 triplets and 80 bytes array { colour, x_min, x_max; + ...

//Energy_communication setup
char receivedData_Energy[3];
char charge_flag = 0;
char current_drawn[2];

void setup()
{
    //Debug communication UART port
    Serial.begin(115200);

    //Energy_communication UART port
    Serial1.begin(115200, SERIAL_8N1, 22, 23);

    //Drive_communication UART port
    Serial2.begin(9600, SERIAL_8N1, 16, 17);

    //start the SPI library:
    pinMode(ss, OUTPUT);
    SPI.begin();

    //Server_communication WiFi connection
    // WiFi.begin(ssid, password);
    // Serial.println("Connecting");

    // while (WiFi.status() != WL_CONNECTED)
    // {
    //     delay(500);
    //     Serial.print(".");
    // }

    // Serial.println("");
    // Serial.print("Connected to WiFi network with IP Address: ");
    // Serial.println(WiFi.localIP());

    //dual_core setup
    xTaskCreatePinnedToCore(Task1code, "Task1", 10000, NULL, 1, NULL,  0); 
    xTaskCreatePinnedToCore(Task2code, "Task2", 10000, NULL, 1, NULL,  1);
}

void loop() {}



//Drive-Vision-Energy-Server communication
void Task1code(void *parameter)
{
    for (;;)
    {
        StaticJsonDocument<2048> JSON_Rover_to_Server;
        StaticJsonDocument<96> JSON_Server_to_Rover;

        //receive_data_Drive();
        update_JSON_obj_Drive(JSON_Rover_to_Server);

        //receive_data_Energy();
        update_JSON_obj_Energy(JSON_Rover_to_Server);

        receive_data_Vision();
        update_JSON_obj_Vision(JSON_Rover_to_Server);

        //POST_data_Server(JSON_Rover_to_Server, JSON_Server_to_Rover);

        //send_position_data_Rover(JSON_Server_to_Rover);
        delay(1000);
    }
}

//Server-Energy communication
void Task2code(void *parameter)
{ 
    for (;;)
    {
        //send_energy_data_Rover();
        delay(300);
    }
}



/////////// Drive communication helper functions ////////////
void receive_data_Drive()
{
    static bool recvInProgress_Drive = false;
    static byte counter_Drive = 0;
    char SOM = '{';
    char EOM = '}';
    char received_byte;

    while (true)
    {
        while (Serial2.available() > 0)
        {
            received_byte = Serial2.read();
            //discard bytes until SOM is encounterd
            if (received_byte == SOM)
            {
                recvInProgress_Drive = true;
                continue;
            }

            //receive data
            if (recvInProgress_Drive == true)
            {
                if (received_byte == EOM)
                {
                    recvInProgress_Drive = false;
                    counter_Drive = 0;
                    return;
                }
                receivedData_Drive[counter_Drive] = received_byte;
                counter_Drive++;
            }
        }
    }
}

void update_JSON_obj_Drive(JsonDocument &JSON_Rover_to_Server)
{
    uint8_t polar_depth = receivedData_Drive[0];
    uint8_t polar_angle = receivedData_Drive[1];
    uint8_t delta_theta = receivedData_Drive[2];
    
    //update global variable for Energy communication
    current_drawn[0] = receivedData_Drive[3];
    current_drawn[1] = receive_data_Drive[4];

    Serial.println("data received from Drive");
    Serial.print("polar_depth is: ");
    Serial.println(polar_depth);
    Serial.print("polar_angle is: ");
    Serial.println(polar_angle);
    Serial.print("delta_theta is: ");
    Serial.println(delta_theta);

    JSON_Rover_to_Server["polar_depth"] = polar_depth;
    JSON_Rover_to_Server["polar_angle"] = polar_angle;
    JSON_Rover_to_Server["delta_theta"] = delta_theta;
}

/////////// Vision communication helper functions ////////////
void receive_bytes_Vision()
{
    static bool recvInProgress_Vision = false;
    static byte counter_Vision = 0;
    char SOM = '{';
    char EOM = '}';
    char received_byte;

    Serial.println("data received from vision: ");
    while (true)
    {
        received_byte = SPI.transfer(0);
        
        //
        if(received_byte == 0) continue;
        Serial.println((int)received_byte);
        //discard bytes until SOM is encounterd
        if (received_byte == SOM)
        {
            recvInProgress_Vision = true;
            continue;
        }

        //receive data
        if (recvInProgress_Vision == true)
        {
            if (received_byte == EOM)
            {
                recvInProgress_Vision = false;
                counter_Vision = 0;
                Serial.println("return");
                Serial.println("end of data received from vision");
                return;
            }
            receivedData_Vision[counter_Vision] = received_byte;
            counter_Vision++;
        }
    }
}

void receive_data_Vision()
{
    SPI.beginTransaction(settingsA);

    digitalWrite(ss, LOW);
    receive_bytes_Vision();
    digitalWrite(ss, HIGH);

    SPI.endTransaction();
}

void update_JSON_obj_Vision(JsonDocument &JSON_Rover_to_Server)
{
    JsonArray Vision_data = JSON_Rover_to_Server.createNestedArray("balls");

    for (int i = 0; i < 3 * 5; i += 3)
    {
        JsonObject triplet = Vision_data.createNestedObject();
        String colour; 
        if(receivedData_Vision[i] == 1) colour = "pink";
        else if(receivedData_Vision[i] == 2) colour = "green";
        else if(receivedData_Vision[i] == 3) colour = "black";
        else if(receivedData_Vision[i] == 4) colour = "blue";
        else if(receivedData_Vision[i] == 5) colour = "orange";
        triplet["colour"] = colour;
        triplet["min_x"] = (int)((unsigned char)receivedData_Vision[i + 1]);
        triplet["max_x"] = (int)((unsigned char)receivedData_Vision[i + 2]);
    }

    JsonArray depth = JSON_Rover_to_Server.createNestedArray("depth");

    for (int i = 15; i < 95; i++)
    {
        depth.add((int)((unsigned char)receivedData_Vision[i]));
    }
}

/////////// Server communication helper functions ////////////
void POST_data_Server(JsonDocument &JSON_Rover_to_Server, JsonDocument &JSON_Server_to_Rover)
{

    //Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        String serverPath = serverName;

        http.useHTTP10(true);
        // Domain name with URL path or IP address with path
        http.begin(serverPath.c_str());
        http.addHeader("Content-Type", "application/json");

        // Send HTTP POST request
        String json;
        serializeJson(JSON_Rover_to_Server, json);
        int httpResponseCode = http.POST(json);

        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);

        DeserializationError error = deserializeJson(JSON_Server_to_Rover, http.getStream());

        if (error)
        {
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

void send_position_data_Rover(JsonDocument &JSON_Server_to_Rover)
{
    if (!JSON_Server_to_Rover.isNull())
    {
        //extract values from JSON document
        uint8_t polar_depth = JSON_Server_to_Rover["polar_depth"];
        uint8_t polar_angle = JSON_Server_to_Rover["polar_angle"];

        //update global variable for Energy communication
        charge_flag = JSON_Server_to_Rover["charge_flag"];

        Serial.println("data received from Server");
        Serial.print("polar_depth is: ");
        Serial.println(polar_depth);
        Serial.print("polar_angle is: ");
        Serial.println(polar_angle);
        Serial.print("charge_flag is: ");
        Serial.println((int)charge_flag);
       
        //send data to Drive
        Serial2.write('{');
        Serial2.write(polar_depth);
        Serial2.write(polar_angle);
        Serial2.write('}');
    }
    else
    {
        Serial.println("error: JSON_Server_to_Rover is Null");
    }
}

void send_energy_data_Rover()
{
    //send charge flag
    Serial1.write('a');
    //send current_drawn data
    Serial1.write(current_drawn[0]);
    Serial1.write(current_drawn[1]);
}

/////////// Energy communication helper functions ////////////
void receive_data_Energy()
{
    static bool recvInProgress_Energy = false;
    static byte counter_Energy = 0;
    char SOM = '{';
    char EOM = '}';
    char received_byte;

    while (true)
    {
        while (Serial1.available() > 0)
        {
            received_byte = Serial1.read();
            
            //discard bytes until SOM is encounterd
            if (received_byte == SOM)
            {
                recvInProgress_Energy = true;
                continue;
            }

            //receive data
            if (recvInProgress_Energy == true)
            {
                if (received_byte == EOM)
                {
                    recvInProgress_Energy = false;
                    counter_Energy = 0;
                    return;
                }
                receivedData_Energy[counter_Energy] = received_byte;
                counter_Energy++;
            }
        }
    }
}

void update_JSON_obj_Energy(JsonDocument &JSON_Rover_to_Server)
{
    uint8_t SOC = receivedData_Energy[0];
    uint8_t SOH = receivedData_Energy[1];
    uint8_t BatteryWarning_flag = receivedData_Energy[2];

    Serial.println("data from energy:");
    Serial.println((int)receivedData_Energy[0]);
    Serial.println((int)receivedData_Energy[1]);
    Serial.println((int)receivedData_Energy[2]);

    JSON_Rover_to_Server["SOC"] = SOC;
    JSON_Rover_to_Server["SOH"] = SOH;
    JSON_Rover_to_Server["BatteryWarning_flag"] = BatteryWarning_flag;
}