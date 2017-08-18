/*
Mommy Care

Código desarrollado por el equipo de Technomadic para la creación de un dispositivo 
que se utilizará para detectar patrones de riesgo en mujeres embarazadas. Todos los datos
se envían al servidor de Technomadic a la dirección (http://technomadic.westcentralus.cloudapp.azure.com/Mommy_Care/PHP/paciente.php)
utilizando la red celular de Telefónica-Movistar.
El dispositivo cuenta con un teclado alfanumérico para llevar a cabo la selección de parámetros a medir.
También cuenta con una pantalla LCD que muestra un menú y los resultados del análisis. 
Todos los datos se envían de forma automática al servidor.

Para el correcto funcionamiento del dispositivo, es necesario realizar una conexión entre el 
baumanómetro y el sensor de presión cardiaca ASDXRRX015PDAA5.

Para el desarrollo del proyecto se hace uso de la placa BB9, aunque también se puede 
hacer uso de un Arduino UNO y un Módulo de comunicación UC20 de Quectel. (https://tinyurl.com/quecteluc20)


El objetivo principal del proyecto es la detección de cuatro signos vitales:
        *Temperatura corporal: Para la medición de este parámetro se hace uso del sensor TMP007 de Adafruit.
                               Es un sensor de temperatura de Adafruit. Utiliza un sensor infrarrojo para 
                               calcular la temperatura de la persona con gran precisión. (https://tinyurl.com/TMP007-ada)
        *Presión (Distólica y Sistólica): Para hacer medición de la presión arterial del paciente
                               se hace uso del sensor ASDXRRX015PDAA5 de Honeywell (ASDXRRX015PDAA5). Este sensor 
                               hace una comparación entre la presión atmosférica y la presioń leída por el baumanómetro.

        *Pulso: Para la medición de la frecuencia cardiaca se utiliza el sensor SEN-11574 de SparkFun (https://tinyurl.com/PulseSensorSparkFun)
                               Este sensor genera pulsos de luz en un led colocado en la superficie, y un fotosensor para
                               calcular el pulso cardiaco por medio de las variaciones de iluminación que recibe el sensor.

        *Frecuencia respiratoria: Aún está pendiente...

A continuación se muestran las direcciones de la pantalla LCD y el teclado alfanumérico:
        *Pantalla LCD (LiquidCrystal_PCF8574) se puede encontrar en: https://tinyurl.com/LiquidCrystalSparkFun
        *Teclado Alfanumérico: https://tinyurl.com/KeypadArduino

Creado: 21 de Mayo de 2017
por Rogelio Ventura y Juan Antonio Oriza 
en colaboración con Daniel Reyes

*/

//Bloque de librerias#include <MAX17043.h>
#include "Adafruit_TMP007.h" //Librería para sensor de temperatura
#include <Wire.h> //Sensor para comunicación I2C
#include <LiquidCrystal_PCF8574.h> //Sensor de pantalla con protocolo I2C
#include <Keypad.h> //Librería para sensor para teclado alfanumérico de 4x4 (En este código sólo se utiliza una matriz 1x3 para las letras A, B y C)
#include <SoftwareSerial.h> // Librería para comunicación con UC20

//variables generales
int x=0;
char key; //variable de caracter obtenido por el teclado alfanumérico
float real_temp; //variable de temperatura obtenida por el sensor de TMP007
int z; 
int pulso;
int i ;
int estatus_conexion = 0;


//Generacion de Objetos
Adafruit_TMP007 tmp007; //Generación de objeto de sensor de temperatuar
SoftwareSerial Uc20(5,6); //rX, tX Pines de conexión entre BB9 y UC20
const byte ROWS = 3; //Número de filas para la matriz utilizada 
const byte COLS = 1; //Número de columnas para la matriz utilizada

//definición de matriz del teclado
char keys[ROWS][COLS] = {
  {'A'},
  {'B'},
  {'C'},
};
//Definiciín de pines para teclado alfanumérico
byte rowPins[ROWS] = {10, 9, 8}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {11}; //connect to the column pinouts of the keypad

//Definicion de funciones
void sendMsg();

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS ); //Declaración de objeto Teclado
LiquidCrystal_PCF8574 lcd(0x27);  //COnfiguración inicial de pantalla LCD, con dirección 0x27 con dos renglones y 16 caracteres para cada uno

//Configuración inicial de BB9
void setup() {
  pinMode(A2, INPUT);
  int error; // variable utilizada para monitoreo de funcionamiento de pantalla LCD
  Wire.begin(); // Inicio de comunicación I2C
  Serial.begin(9600); //Inicio de Comunicación Serial por medio de pines 2 y 3 (Comentar esta líne de código cuando no se realice debug del código)
 // Uc20.begin(9600); //Inicio de Comunicación Serial con UC20, utilizando pines 5 y 6 (La velocidad debe ser de 9600 para ser compatible con la velocidad de BB9)
  Uc20.begin(115200);
  delay(1000);
  sendATCommand("AT+IPR=9600", 100);
  Uc20.begin(9600);
  delay(1000); 
  Serial.println("QUC20 inicializado a 115200"); //Impresión para Debug (Inicio del código)
  sessionsInit(); //Inicio de sesión en la red telefónica
  Serial.println("LCD..."); //Línea para debug de LCD
  while (! Serial);
  Serial.println("Dose: check for LCD");
  //Wire.begin();
  Wire.beginTransmission(0x27); //iniciar comunicación con pantalla LCD
  error = Wire.endTransmission(); //Terminar comunicación con LCD si no se detecta una pantalla
  lcd.begin(16, 2); //Iniciar pantalla. Se declara el tamaño de LCD
  lcd.setBacklight(255); //Iniciar iluminación de pantalla
  escribir("Mommy Care", "Demo");
  delay(1000);
  lcd.clear();
  escribir("Selec. A:Temp","B:Presion C:Puls"); //Imprimir en LCD menú principal
}

void loop() {
  
  key = keypad.getKey(); //Arduino está listo para recibir caracteres desde el teclado alfanumérico
  //Serial.println(key); Descomentar esta linea para realizar debug del código

  if (key){ //Si el Arduino detecta que una de las teclas es presionada limpia la pantalla LCD

    lcd.clear();
    
    switch (key) { //Dependiendo de la tecla presionada entra en diferentes partes del código
      
      case 'A': //En caso de haber presionado la letra 'A' el Arduino entra en modo de medición de temperatura.
      
        escribir("Coloque sensor","Iniciar: Pres:A"); //Impresión en pantalla LCD
      
        while (x==0){ //Ciclo de medición y envío de datos
      
          key = keypad.getKey(); //Se espera a que el usuario coloque el sensor y presione la letra 'A'
      
          if (key=='A'){
            real_temp = tmp007.readObjTempC();//Se hace la medición de la temperatura
            //Serial.print("Temperatura Detectada: "); Serial.print(real_temp); Serial.println("*C"); //Descomentar esta linea para debug del código
            delay(1000); //Esperar un segundo
            lcd.clear();//Borrar contenido en pantalla LCD
            escribir("Enviando...",""); //Mostrar en pantalla que se están enviando los datos
            for(estatus_conexion=0; estatus_conexion<=1; estatus_conexion++){//Ciclo para envío de datos por medio de la red celular
              sendMsg(); //
            }
            estatus_conexion=0; //reinicio de variable de control para envío de datos
            lcd.clear();
            delay(500);
            lcd.setCursor(0,0);
            lcd.print("Temperatura"); 
            lcd.setCursor(0,1);
            lcd.print(real_temp); //Imprimir temperatura en pantalla LCD por dos segundos
            delay(2000);
            x=1;
          }
        }
        break;
      case 'B': //ciclo para medición de presión arterial (El código es igual para todos los casos, sólo cambia la medición de cada variable)
        escribir("Coloque Bauman","Iniciar: Pres:A");
        while (x==0){
          key = keypad.getKey();
          if (key=='A'){
            delay(1000);
            lcd.clear();
            escribir("Enviando...","");
            for(estatus_conexion=0; estatus_conexion<=1; estatus_conexion++){
              sendMsg();
            }
            estatus_conexion=0;
            x=1;
          }
        }
        break;
      case 'C': //ciclo para medición de pulso cardiaco
        escribir("Coloque dedo","Iniciar: Pres:A");
        while (x==0){
          key = keypad.getKey();
          if (key=='A'){
            for (i = 0 ; i<10; i++){
              int x=map(analogRead(A2), 0, 1024, 100, 50);
              z+=x;
              pulso = z/10;
              delay(100);
            }
            
            delay(1000);
            lcd.clear();
            delay(1000);
            lcd.setCursor(0,0);
            lcd.print("Pulso:");
            lcd.setCursor(0,1);
            lcd.print(pulso);
            delay(1000);
            lcd.clear();
            escribir("Enviando...","");
            for(estatus_conexion=0; estatus_conexion<=1; estatus_conexion++){
              sendMsg();
            }
            estatus_conexion=0;
            x=1;
            z=0;
            pulso=0;
            i=0;
          }
        }
        break;
      default:
        break;
    }
  }
  x=0;
  escribir("Selec. A:Temp","B:Presion C:Puls");
}


//función para escribir en pantalla LCD de forma cíclica
//las variables renglon1 y renglon2 aparecen primero con una duración de 3 seg
//las variables renglon3 y renglon4 aparecen después con la misma duración (las 4 variables no deben revasar los 16 caracteres)
//las variables son de tipo String, por lo que en la función se deben colocar entre comillas
void escribir_ciclo(String renglon1, String renglon2, String renglon3, String renglon4){
  //lcd.clear();
  lcd.setCursor(0,0); //Se coloca el cursor de LCD en inicio
  //lcd.setBacklight(255);
  lcd.print(renglon1); //Se imprime el primer parámetro
  lcd.setCursor(0,1); //Se coloca el sensor en el segundo renglón de la pantalla
  lcd.print(renglon2); //Se imprime el segundo parámetro
  delay(3000);
  lcd.clear(); //Función para dejar la pantalla en blanco
  lcd.setCursor(0,0); //Cursor se coloca en inicio
  //lcd.setBacklight(255);
  lcd.print(renglon3); //Se imprime tercer parámetro
  lcd.setCursor(0,1); //Cursor colocado en segundo renglón
  lcd.print(renglon4); //Se imprimecuarto parámetro
  delay(3000);
}


//Función para escribir en pantalla LCD
//Sólo tiene dos parámetros, ya que no necesita hacer nada, mas que esperar interacción con el usuario.
//Funciona de la misma forma que la funcion escribir_ciclo(), pero no cambia con el tiempo.
void escribir(String renglon1, String renglon2){
  //lcd.clear();
  lcd.setCursor(0,0);
  lcd.setBacklight(255);
  lcd.print(renglon1);
  lcd.setCursor(0,1);
  lcd.print(renglon2);
  delay(500);
}

//Función para iniciar sesión en red celular
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
  res += real_temp;
  res += "&presion_dis=0&presion_sis=0&pulso=";
  res+=pulso;
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







