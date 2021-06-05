#include <stdint.h>

//Control_communication setup
char receivedData_Control[3];

void setup()
{
    Serial.begin(115200);
    Serial1.begin(9600);
}

void loop()
{
    send_data_Control();
    delay(5000);
    //receive_data_Control();
    //print_data_Control();
}

void send_data_Control()
{
    //insert code to compute variables
    uint8_t SOC = 12;
    uint8_t SOH = 3;
    uint8_t BatteryWarning_flag = 7;

    Serial1.write('{');
    Serial1.write(SOC);
    Serial1.write(SOH); 
    Serial1.write(BatteryWarning_flag);
    Serial1.write('}');
}

void receive_data_Control()
{
    static bool recvInProgress_Control = false;
    static byte counter_Control = 0;
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
                recvInProgress_Control = true;
                continue;
            }

            //receive data
            if (recvInProgress_Control == true)
            {
                if (received_byte == EOM)
                {
                    recvInProgress_Control = false;
                    counter_Control = 0;
                    return;
                }
                receivedData_Control[counter_Control] = received_byte;
                counter_Control++;
            }
        }
    }
}