#include <SoftwareSerial.h> // Librería para comunicación con UC20
#include <Wire.h> //Librer[ia para comunicaci[on de I2C]
#include "Adafruit_TMP007.h" //Librería para sensor de temperatura


SoftwareSerial Uc20(2, 3);

int receive_data;
float real_temp;
int pulso;
Adafruit_TMP007 tmp007; //Generación de objeto de sensor de temperatuar
int z;
int map_pulso;


void setup(){
  int error;
  pinMode(A2, INPUT);
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
  Wire.beginTransmission(0x27); //iniciar comunicación con pantalla LCD
  error = Wire.endTransmission(); //Terminar comunicación con LCD si no se detecta una pantalla

  
}


void loop(){
  delay(100);
  if (receive_data!=0){
    switch (receive_data) {
      case 1:
        Serial.println("Tomar temperatura");
        real_temp = tmp007.readObjTempC();//Se hace la medición de la temperatura
        Serial.println(real_temp);
        receive_data=0;
        requestEvent(real_temp);
        break;
      case 2:
        Serial.println("Tomar presion arterial");
        for (int i = 0 ; i<10; i++){
          map_pulso=map(analogRead(A2), 0, 1024, 100, 50);
          z+=map_pulso;
          delay(100);
        }
        pulso = z/10;
        Serial.println(pulso);
        z+0;
        receive_data=0;
        break;
      case 3:
        Serial.println("Tomar glucosa");
        receive_data=0;
        break;
      case 4:
        Serial.println("Tomar frecuencia respiratoria");
        receive_data=0;
        break;
      case 5:
        Serial.println("Tomar frecuencia cardiaca");
        receive_data=0;
        break;
      case 6:
        Serial.println("Tomar cardaica fetal");
        receive_data=0;
        break;
      default:
        Serial.println("En espera");
    }
  }
}

void requestEvent(fString dato) {
  Wire.write(dato); // respond with message of 6 bytes
  // as expected by master
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