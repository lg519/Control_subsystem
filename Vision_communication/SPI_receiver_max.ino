#include <SPI.h>

int ss = 14;

SPISettings settingsA(2000000, LSBFIRST, SPI_MODE1);

 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  // start the SPI library:
  pinMode(ss, OUTPUT); //ss is pin 10
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV4);
  digitalWrite(ss,HIGH);
}

 

void loop() {
  
  delay(1000);
  
  SPI.beginTransaction(settingsA);
  
  //receive message
  digitalWrite(ss,LOW);
  char response[30];
  int counter = 0;
  char data_ = SPI.transfer(0);
  Serial.print(data_);
  response[counter] = (char)data_;
  counter++;
  while(true){
     data_ = SPI.transfer(0);
     response[counter] = data_;
     counter++;
     Serial.print(data_);
  }
  digitalWrite(ss,HIGH);
  //finish receive message

  SPI.endTransaction();

  //print message to terminal
  Serial.println("Recieved message:");
  for(int i = 0; i < counter; i++){
  //  Serial.print(response[i]);
  }
  Serial.println("\nMessage ended");

  
}