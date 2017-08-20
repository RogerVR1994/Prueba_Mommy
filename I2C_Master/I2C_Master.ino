#include <Wire.h>

void setup() {
  Wire.begin(); 
  Serial.begin(9600);  // start serial for output
}

byte x = 0;

void loop() {
  Wire.beginTransmission(8); 
  Wire.write("x is ");       
  Wire.write(x);             
  Wire.endTransmission();    

  x++;
  delay(500);
  Wire.requestFrom(8, 5);    // request 6 bytes from slave device #8

  while (Wire.available()) { // slave may send less than requested
    char c = Wire.read(); // receive a byte as character
    Serial.print(c);         // print the character
  }

  delay(500);

}