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
int frecuencia_respiratoria;
int glucosa;
int presion_sis;
int presion_dis;
int contracciones;
int frecuencia_fetal;
String dato;

long previousMillis = 0;
unsigned long currentMillis = 0;
int totalSend = 0;
boolean uConfig = false;
int gnAct = 0;
String post_message="";

void sendMsg();


void setup(){
  int error;
  pinMode(A2, INPUT);
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);
  delay(100);
  digitalWrite(5, LOW);
  Serial.begin(9600);
  Uc20.begin(9600);
  Wire.begin(8);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent); // register event
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
    frecuencia_respiratoria = random(16, 20);
    switch (receive_data) {
      case 1:
        Serial.println("Tomar temperatura");
        real_temp = tmp007.readObjTempC();//Se hace la medición de la temperatura
        Serial.println(real_temp);
        receive_data=0;
        real_temp=random(36.01, 37.1);
        dato= String(real_temp);
        post_message="temperatura=";
        post_message+= dato;
        break;
      default:
        Serial.println("En espera");
    }
    delay(1000);
    for (int status=0; status<2; status++){

      sendMsg(post_message);
      delay(1000);
    }
    Serial.println(post_message);
    post_message="";
  }
}

void requestEvent() {
  
  switch (receive_data) {
      case 2:
        for (int i = 0 ; i<10; i++){
          map_pulso=map(analogRead(A2), 0, 1024, 100, 50);
          z+=map_pulso;
          delay(100);
        }
        pulso = (z/10)-17;
        //Serial.println(pulso);
        dato=String(pulso);
        //Serial.println("hola");
        //Serial.println(dato);
        z=0;
        frecuencia_respiratoria=random(16, 20);
        Serial.print("frecuencia");
        Serial.println(frecuencia_respiratoria);
        dato+=" ";
        dato+=String(frecuencia_respiratoria);
        post_message="frecuencia_respiratoria=";
        post_message += frecuencia_respiratoria;
        post_message+="&pulso=";
        post_message += pulso;
        
        receive_data=0;
        break;
      case 3:
        glucosa = random(16, 20);
        Serial.println("glucosa");
        Serial.println(glucosa);
        dato = String(glucosa);
        
        post_message="glucosa=";
        post_message+=glucosa;
        Serial.println(dato);
        receive_data=0;
        break;
      case 4:
        presion_dis = random(60, 80);
        presion_sis = random(100, 120);
        Serial.println("presion sistolica");
        Serial.println(presion_sis);
        Serial.println("presion distolica");
        Serial.println(presion_dis);
        dato = String(presion_sis);
        dato += "/";
        dato += String(presion_dis);
        Serial.println(sizeof(dato));
        post_message="presion_dis=";
        post_message+= presion_dis;
        post_message+="&presion_sis=";
        post_message+= presion_sis;
        
        receive_data=0;
        break;
      case 5:
        contracciones = random(1, 2);
        dato = String(contracciones);
        receive_data=0;
        post_message="contracciones=";
        post_message+= contracciones;
        break;
      case 6:
        frecuencia_fetal = random(160, 170);
        dato = String(frecuencia_fetal);
        receive_data=0;
        post_message="frecuencia_fetal=";
        post_message+= frecuencia_fetal;
        break;
      default:
        Serial.println("algo");// do something
  }
  char data[7];
  dato.toCharArray(data, 7);
  Serial.println(data);
  Wire.write(data); // respond with message of 6 bytes
  // as expected by master
  dato="";
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
  sendATCommand("AT+QICSGP=1,1,\"internet.itelcel.com\",\"webgprs\",\"webgprs2002\",1", 100);
  //sendATCommand("AT+QICSGP=1,1,\"internet.movistar.mx\",\"movistar\",\"movistar\",1", 100);
  sendATCommand("AT+QIACT=1", 100);
}


void sessionsClose() {
  sendATCommand("AT+QGPSEND", 100); //Termina sesion de GPS
  sendATCommand("AT+QIDEACT=1", 100); //Termina sesion de 3G
  delay(1000);
}

void restartUC() {
  sessionsClose();
  sendATCommand("AT+QGPSEND", 100);
  sendATCommand("AT+QIDEACT=1", 100);
  delay(1000);
}

void powerOff() {
  sessionsClose();
  sendATCommand("AT+QPOWD", 300);
}


//Funcion de envio de comandos AT
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

//Funcion para obtener un campo en particular de una cadena csv
String parseCSV(String csv, int field) {
  String sField;
  int fCom, sCom;
  for (int i = 0; i < field; i++) {
    fCom = csv.indexOf(',');
    sCom = csv.indexOf(',', fCom + 1);
    sField = csv.substring(fCom + 1, sCom);
    csv.remove(fCom, sField.length());
  }
  return sField;
}

//Funcion de envio de datos a traves de 3G
void sendMsg(String mensaje) {
  String act;
  String res, atcomm;
  res = "temperatura=36.9&presion_dis=0&presion_sis=0&pulso=71";
  sendATCommandWithResponse("AT+QHTTPURL=48,48", "http://52.161.31.218/Mommy_Care/PHP/add_data.php");
  delay(30);
  sendATCommand("AT+QIGETERROR", 100);
  atcomm = "AT+QHTTPPOST=";
  atcomm += mensaje.length();
  atcomm += ",80,80";
  Serial.println(atcomm);
  sendATCommandWithResponse(atcomm, mensaje);
  delay(30);
  sendATCommand("AT+QIGETERROR", 100);
  delay(20);
  Serial.println(sendATCommand("AT+QHTTPREAD=80", 100));
  delay(30);
  sendATCommand("AT+QHTTPREAD=30",100);
  delay(30);
  Serial.println(mensaje);
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


