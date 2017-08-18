#include <SoftwareSerial.h>

SoftwareSerial MCU(7, 6);
char incoming_char;


void setup(){
	Serial.begin(9600);
	MCU.begin(9600);
}

void loop(){
	getData();
}

void getData() {
  	String data = "";
	char incoming_char;
	Serial.println(MCU.available());
	while (MCU.available() > 0) {
		incoming_char = MCU.read();
    	Serial.print(incoming_char);
    	data += incoming_char;
  	}
  	data.remove(1);
  	if (data=="1"){
  		Serial.println("Success!");
  	}
  	delay(500);
  	Serial.println(data);
  	Serial.println("Tamaño del dato:");
  	Serial.println(data.length());
  	Serial.println("");
  	Serial.println("");
  	Serial.println("");
}