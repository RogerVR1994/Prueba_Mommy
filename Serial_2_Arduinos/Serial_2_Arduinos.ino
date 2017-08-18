
void setup(){
	Serial.begin(9600);
}

void loop(){
	Serial.println("hola");
	sendData("1", 100);
}

String sendData(String command, int ms) {
  char c;
  String res;
  Serial.println(command);     
  int count = 5;                      
  // assuming there is no more data
  while (count-- != 0) {               

    delay(100);                       

    while (Serial.available() > 0) { 
      c = (char)Serial.read();     
      res += c;
      count = 5;
    }
  }
  Serial.println(res);
  return res;
}