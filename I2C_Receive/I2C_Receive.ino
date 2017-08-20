#include <Wire.h>

void setup() {
  Wire.begin(8);                
  Wire.onReceive(receiveEvent); 
  Serial.begin(9600);          
}

void loop() {
  delay(100);
}

void receiveEvent(int howMany) {
  while (1 < Wire.available()) { 
    char c = Wire.read(); 
    Serial.print(c);        
  }
  int x = Wire.read(); 
  int y=1;   
  Serial.println(x);         
  Wire.beginTransmission(0);    
  Wire.write(y);             
  Wire.endTransmission();   
}