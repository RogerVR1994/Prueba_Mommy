#include <Wire.h>

void setup() {
  Wire.begin(8);                
  Wire.onReceive(receiveEvent); 
  Wire.onRequest(requestEvent); // register event
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
  Serial.println(x);         
}

void requestEvent() {
  Wire.write("hola "); // respond with message of 6 bytes
  // as expected by master
}