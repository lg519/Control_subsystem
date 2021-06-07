//Energy_communication setup
char receivedData_Energy[3];
char charge_flag = 0;

void setup()
{
    //Debug communication UART port
    Serial.begin(115200);

    //Energy_communication UART port
    Serial1.begin(115200, SERIAL_8N1, 22, 23);

    //dual_core setup
    xTaskCreatePinnedToCore(Task1code, "Task1", 10000, NULL, 1, NULL,  0); 
    xTaskCreatePinnedToCore(Task2code, "Task2", 10000, NULL, 1, NULL,  1);
}

void loop() 
{
    
}

void Task1code(void *parameter)
{
    for (;;)
    {
        receive_data_Energy();
        Serial.print("SOC is :");
        Serial.println((int)receivedData_Energy[0]);
        Serial.print("SOH is :");
        Serial.println((int)receivedData_Energy[1]);
        Serial.print("BatteryWarning_Flag is :");
        Serial.println((int)receivedData_Energy[2]);
        delay(1);
    }
}

//Server-Energy communication
void Task2code(void *parameter)
{ 
    for (;;)
    {
        send_energy_data_Rover();
    }
}

void send_energy_data_Rover()
{
    //send data to Energy
    //Serial.println("send data to smartin");
    Serial1.write('{');
    Serial1.write('e');
    Serial1.write('}');
    Serial1.flush();
    delay(100);
}

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