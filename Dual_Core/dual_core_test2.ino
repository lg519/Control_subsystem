void setup()
{
    Serial.begin(115200);
    

    xTaskCreatePinnedToCore(Task1code, "Task1", 10000, NULL, 1, NULL,  0); 
    xTaskCreatePinnedToCore(Task2code, "Task2", 10000, NULL, 1, NULL,  1); 
}

void loop() {}

int shared_array[1] = {8};

void Task1code(void *parameter)
{
    for (;;)
    {
        //infinite loop
        Serial.print("Task1 running on core ");
        Serial.println(xPortGetCoreID());

        Serial.print("shared array value Task1 is: ");
        Serial.println(shared_array[0]);
        shared_array[0] = 1;
        delay(7000);
    }
}

void Task2code(void *pvParameters)
{ 
    for (;;)
    {
        Serial.print("Task2 running on core ");
        Serial.println(xPortGetCoreID());

        Serial.print("shared array value Task2 is: ");
        Serial.println(shared_array[0]);
        shared_array[0] = 2;
        delay(4000);
    }
}