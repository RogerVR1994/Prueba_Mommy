#include <Wire.h>

void setup() {
  Wire.begin(); 
  Wire.onReceive(receiveEvent); 
  Serial.begin(9600);  
}

byte x = 0;

void loop() {
  Wire.beginTransmission(8); 
  Wire.write("x is ");       
  Wire.write(x);             
  Wire.endTransmission();    

  x++;
  delay(500);
}
void receiveEvent(int howMany) {
  while (1 < Wire.available()) { 
    char c = Wire.read(); 
    Serial.print(c);        
  }
  int x = Wire.read();    
  Serial.println(x);         
}