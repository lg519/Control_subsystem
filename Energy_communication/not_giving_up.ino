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
    Serial.println("send data to smartin");
    Serial1.write('{');
    Serial1.write('h');
    Serial1.write('e');
    Serial1.write('l');
    Serial1.write('l');
    Serial1.write('o');
    Serial1.write('}');
    Serial1.flush();
    delay(100);
}