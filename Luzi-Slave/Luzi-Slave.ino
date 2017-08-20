#include <SoftwareSerial.h> // Librería para comunicación con UC20
#include <Wire.h> //Librer[ia para comunicaci[on de I2C]

SoftwareSerial Uc20(2, 3);

int receive_data;

void setup(){
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);
  Serial.begin(9600);
  Wire.begin(8);
  Wire.onReceive(receiveEvent);
  sendATCommand("AT+IPR=9600", 100);
  Uc20.begin(9600);
  delay(1000); 
  Serial.println("QUC20 inicializado a 115200"); //Impresión para Debug (Inicio del código)
  sessionsInit(); //Inicio de sesión en la red telefónica
  
}


void loop(){
  delay(100);
  if (receive_data==1){
    Serial.println("exito!");
    receive_data=0;
    for (int count = 0; count<2; count++){
      sendMsg();
    }
  }
}

void receiveEvent(int howmany){
  while(1 < Wire.available()){
    char c = Wire.read();
    Serial.println(c);
  }
  receive_data = Wire.read();
  Serial.println(receive_data);
}

void sessionsInit() {
  //Activar GPS
  sendATCommand("AT+QGPS=1", 100);
  //Activar conexion 3G
  sendATCommand("AT+QHTTPCFG=\"contextid\",1", 100);
  sendATCommand("AT+QHTTPCFG=\"responseheader\",1", 100);
  //sendATCommand("AT+QICSGP=1,1,\"internet.itelcel.com\",\"webgprs\",\"webgprs2002\",1", 100);
  sendATCommand("AT+QICSGP=1,1,\"internet.movistar.mx\",\"movistar\",\"movistar\",1", 100); //Se colocan los datos de la tarjeta SIM
  sendATCommand("AT+QIACT=1", 100);
}

//Funcion de envio de comandos AT a UC20
String sendATCommand(String command, int ms) {
  char c;
  String res;
  Uc20.println(command);     
  int count = 5;                      
  // assuming there is no more data
  while (count-- != 0) {               

    delay(100);                       

    while (Uc20.available() > 0) { 
      c = (char)Uc20.read();     
      res += c;
      count = 5;
    }
  }
  Serial.println(res);
  return res;
}


//Limpia una respuesta de un comando para dejar solo la respuesta
String getBodyResponse(String msg) {
  int startW = 0, endsW = 0;
  String bodyRes;

  startW = msg.indexOf('\n');
  endsW = msg.indexOf('\n', startW + 1);
  return msg.substring(startW + 1, endsW);
}

//Limpia una respuesta de un comando para dejar solo la respuesta
//Esta sobrecarga permite lidiar con las diferencias entre comandos
//de GPS y de Datos 3G
String getBodyResponse(String msg, int mode) {
  int startW = 0, endsW = 0;
  String bodyRes;

  startW = msg.indexOf('\n');
  //Serial.println(startW);
  if (mode == 1) {
    endsW = msg.indexOf('\n', startW + 2);
  } else {
    endsW = msg.indexOf('\n', startW + 1);
  }
  //Serial.println(endsW);
  return msg.substring(startW + 1, endsW);
}

//funcion para quitar la cabecera de una respuesta a un comando
String getDataResponse(String data) {
  int ndx = 0;
  ndx = data.indexOf(':');
  data.trim();
  return data.substring(ndx + 1);
}

//Limpia una respuesta de un comando para dejar solo la respuesta
//Esta sobrecarga permite lidiar con los formatos de respuestas 
//a comandos que necesitaban argumentos
String getBodyReadResponse(String msg) {
  int startW = 0, endsW = 0, fLn = 0;
  String bodyRes;

  fLn = msg.indexOf('\n');
  startW = msg.indexOf('\n', fLn + 1);
  endsW = msg.indexOf('\n', startW + 1);
  return msg.substring(startW + 1, endsW);
}

//Función para envío de datos por método POST a una dirección
void sendMsg() {
  String act;
  String res, atcomm; //variable res representa los datos que se enviarán al servidor. Deben tener formato raw (var1=n&var2=n2....varn=nn)
  res = "temperatura=";
  res += "35.5";
  res += "&presion_dis=70&presion_sis=120&pulso=";
  res+="70.0";
  Serial.println(res); //imprimir en serial el valor de la cadena a enviar (Comentar esta cadena cuando ya no se necesita hacer debug)
  sendATCommandWithResponse("AT+QHTTPURL=48,48", "http://52.161.31.218/Mommy_Care/PHP/add_data.php");
  delay(300);
  sendATCommand("AT+QIGETERROR", 100);
  atcomm = "AT+QHTTPPOST=";
  atcomm += res.length();
  atcomm += ",80,80";
  Serial.println(atcomm);
  sendATCommandWithResponse(atcomm, res);
  delay(300);
  sendATCommand("AT+QIGETERROR", 100);
  delay(200);
  Serial.println(sendATCommand("AT+QHTTPREAD=80", 100));
  delay(300);
  sendATCommand("AT+QHTTPREAD=30",100);
  delay(300);
}

//Funcion para enviar comandos que necesiten argumentos una vez que son enviados
void sendATCommandWithResponse(String command, String response) {
  char incoming_char;
  Uc20.println(command);
  delay(500);
  Serial.println(Uc20.available());
  while (Uc20.available() > 0) {
    incoming_char = Uc20.read();
    Serial.print(incoming_char);
  }
  Uc20.println(response);
  delay(300);
  Serial.println(Uc20.available());
  while (Uc20.available() > 0) {
    incoming_char = Uc20.read();
    Serial.print(incoming_char);
  }
  delay(500);
}