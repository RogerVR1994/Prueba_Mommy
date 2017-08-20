#include <UTFT.h>
#include <URTouch.h>
#include <Wire.h>



UTFT    myGLCD(ITDB32S,38,39,40,41);
URTouch  myTouch( 6, 5, 4, 3, 2);
extern uint8_t BigFont[];

int x, y;
char stCurrent[20]="";
int stCurrentLen=0;
char stLast[20]="";
String dato;

/////////VARIABLES GLOBALES////////
char boton;
byte xbyte=0;

void drawButtons()
{
  /*1*/
  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (70,20,120,70);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (70,20,120,70);
  myGLCD.print("1",87,37);

  /*2*/
  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (130,20, 180, 70);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (130,20, 180, 70);
  myGLCD.print("2",147,37);

  /*3*/
  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (190,20, 240, 70);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (190,20, 240, 70);
  myGLCD.print("3",207,37);

  /*4*/
  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (70,80, 120, 130);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (70,80, 120, 130);
  myGLCD.print("4",87,97);

   /*5*/
  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (130,80, 180, 130);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (130,80, 180, 130);
  myGLCD.print("5",147,97);

   /*6*/
  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (190,80, 240, 130);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (190,80, 240, 130);
  myGLCD.print("6",207,97);

  /*Boton enviar*/
  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (90, 150, 220, 200);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (90, 150, 220, 200);
  myGLCD.print("Enviar", 110, 165);
  myGLCD.setBackColor (0, 0, 0);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void updateStr(int val)
{
  if (stCurrentLen<1)
  {
    stCurrent[stCurrentLen]=val;
    stCurrent[stCurrentLen+1]='\0';
    stCurrentLen++;
    myGLCD.setColor(0, 255, 0);
    myGLCD.print(stCurrent, CENTER, 224);
    
  }
  else
  {
    myGLCD.setColor(0, 0, 0); //Incia secuencia de animación de envio de dato
    myGLCD.fillRect(0, 223, 319, 239);
    myGLCD.setColor(255, 255, 255);
    myGLCD.print("Solo una opcion", CENTER, 206);
    delay(500);
    myGLCD.print("                   ", CENTER, 206);
    delay(500);
    myGLCD.print("Solo una opcion", CENTER, 206);
    delay(500);
    myGLCD.print("                  ", CENTER, 206);
    stCurrentLen=0;
    stCurrent[stCurrentLen]=val;
    stCurrent[stCurrentLen+1]='\0';
    stCurrentLen++;
    myGLCD.setColor(0, 255, 0);
    myGLCD.print(stCurrent, CENTER, 224);
    
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Draw a red frame while a button is touched
void waitForIt(int x1, int y1, int x2, int y2)
{
  myGLCD.setColor(255, 0, 0);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
  while (myTouch.dataAvailable())
    myTouch.read();
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
}

/*************************
**  Required functions  **
*************************/

void setup()
{
// Initial setup
  Wire.begin();
  myGLCD.InitLCD();
  myGLCD.setFont(BigFont);
  myGLCD.clrScr();

  myTouch.InitTouch();
  myTouch.setPrecision(PREC_MEDIUM);

  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(0, 0, 255);
  //drawButtons();  
}

void loop()

/**************************PANTALLA HOLA******************************************/
{
  myGLCD.clrScr();

// Draw some lines in a pattern
  myGLCD.setColor (255,0,0);
  for (int i=15; i<224; i+=5)
  {
    myGLCD.drawLine(1, i, (i*1.44)-10, 224);
  }
  myGLCD.setColor (255,0,0);
  for (int i=224; i>15; i-=5)
  {
    myGLCD.drawLine(318, i, (i*1.44)-11, 15);
  }
  myGLCD.setColor (0,255,255);
  for (int i=224; i>15; i-=5)
  {
    myGLCD.drawLine(1, i, 331-(i*1.44), 15);
  }
  myGLCD.setColor (0,255,255);
  for (int i=15; i<224; i+=5)
  {
    myGLCD.drawLine(318, i, 330-(i*1.44), 224);
  }
  myGLCD.setColor(0,255,255);
  myGLCD.print("LUZI", CENTER, 110);
  delay(3000);
  
  bandera:
  myGLCD.clrScr();
  delay(500);
  drawButtons();

/***************************MENU*****************************************/
  while (true)
  {
    if (myTouch.dataAvailable())
    {
      myTouch.read();
      x=myTouch.getX();
      y=myTouch.getY();

      /*Opciones con Botones*/
      if ((y>=20) && (y<=70))  // Upper row //myGLCD.fillRoundRect (70,20,120,70);
      {
        if ((x>=70) && (x<=120))  // Button: 1
        {
          waitForIt(70,20,120,70);
          updateStr('1');
          boton= 'A'; 
        }

         if ((x>=130) && (x<=180))  // Button: 2
        {
          waitForIt(130,20, 180, 70);
          updateStr('2');
          boton= 'B';
        }

         if ((x>=190) && (x<=240))  // Button: 3
        {
          waitForIt(190,20, 240, 70);
          updateStr('3');
          boton= 'C';
        }
      }

      if ((y>=80) && (y<=130))
       {
         if ((x>=70) && (x<=120))  // Button: 4
        {
          waitForIt(70,80, 120, 130);
          updateStr('4');
          boton= 'D';
        }

         if ((x>=130) && (x<=180))  // Button: 5
        {
          waitForIt(130,80, 180, 130);
          updateStr('5');
          boton= 'E';
        }

         if ((x>=190) && (x<=240))  // Button: 6
        {
          waitForIt(190,80, 240, 130);
          updateStr('6');
          boton= 'F';
        }
      }

      /*Boton enviar*/  
      if ((y>=150) && (y<=200))  // Upper row
      {
        if ((x>=90) && (x<=220))  // Boton
        {
            
          waitForIt(90, 150, 220, 200);
          stCurrent[0]='\0';
          stCurrentLen=0;
          
          myGLCD.setColor(0, 0, 0); //Incia secuencia de animación de envio de dato
          myGLCD.fillRect(0, 223, 319, 239);
          //myGLCD.fillRect(0, 224, 319, 239);

           myGLCD.clrScr();         

           for (int i=0; i<8000; i++)
            {
            myGLCD.setColor(random(255), random(255), random(255));
            myGLCD.drawPixel(2+random(316), 16+random(209));
            }
          
          switch(boton){

            case 'A': //Boton 1
            delay(500);
            xbyte=1;
            Wire.beginTransmission(8);
            Wire.write(xbyte);
            Wire.endTransmission();
            myGLCD.fillScr(0, 0, 255);
            myGLCD.setColor(255, 0, 0);
            myGLCD.fillRoundRect(80, 70, 239, 169);
            myGLCD.setColor(255, 255, 255);
            myGLCD.setBackColor(255, 0, 0);
            Wire.requestFrom(8, 5);
            dato ="";
            while (Wire.available()) { // slave may send less than requested
              char c = Wire.read(); // receive a byte as character
              Serial.print(c);         // print the character
              dato +=c;
            }
            myGLCD.print(dato, CENTER, 93); //Insertar variable sensada
            myGLCD.print("LUZI 2017", CENTER, 119);
            delay (3000);
            myGLCD.setBackColor(0, 0, 255);
            break; //Fin Boton 1

            case 'B': //Boton 2
            delay(500);
            xbyte=2;
            Wire.beginTransmission(8);
            Wire.write(xbyte);
            Wire.endTransmission();
            myGLCD.fillScr(0, 0, 255);
            myGLCD.setColor(255, 0, 0);
            myGLCD.fillRoundRect(80, 70, 239, 169);
            myGLCD.setColor(255, 255, 255);
            myGLCD.setBackColor(255, 0, 0);
            Wire.requestFrom(8, 7);
            dato ="";
            while (Wire.available()) { // slave may send less than requested
              char c = Wire.read(); // receive a byte as character
              Serial.print(c);         // print the character
              dato +=c;
            }
            myGLCD.print(dato, CENTER, 93); //Insertar variable sensada
            myGLCD.print("LUZI 2017", CENTER, 119);
            delay (3000);
            myGLCD.setBackColor(0, 0, 255);
            break; //Fin Boton 2

            case 'C': //Boton 3
            delay(500);
            xbyte=3;
            Wire.beginTransmission(8);
            Wire.write(xbyte);
            Wire.endTransmission();
            myGLCD.fillScr(0, 0, 255);
            myGLCD.setColor(255, 0, 0);
            myGLCD.fillRoundRect(80, 70, 239, 169);
            myGLCD.setColor(255, 255, 255);
            myGLCD.setBackColor(255, 0, 0);
            Wire.requestFrom(8, 2);
            dato ="";
            while (Wire.available()) { // slave may send less than requested
              char c = Wire.read(); // receive a byte as character
              Serial.print(c);         // print the character
              dato +=c;
            }
            myGLCD.print(dato, CENTER, 93); //Insertar variable sensada
            myGLCD.print("LUZI 2017", CENTER, 119);
            delay (3000);
            myGLCD.setBackColor(0, 0, 255);
            break; //Fin Boton 3

            case 'D': //Boton 4
            delay(500);
            xbyte=4;
            Wire.beginTransmission(8);
            Wire.write(xbyte);
            Wire.endTransmission();
            myGLCD.fillScr(0, 0, 255);
            myGLCD.setColor(255, 0, 0);
            myGLCD.fillRoundRect(80, 70, 239, 169);
            myGLCD.setColor(255, 255, 255);
            myGLCD.setBackColor(255, 0, 0);
            Wire.requestFrom(8, 7);
            dato ="";
            while (Wire.available()) { // slave may send less than requested
              char c = Wire.read(); // receive a byte as character
              Serial.print(c);         // print the character
              dato +=c;
            }
            myGLCD.print(dato, CENTER, 93); //Insertar variable sensada
            myGLCD.print("LUZI 2017", CENTER, 119);
            delay (3000);
            myGLCD.setBackColor(0, 0, 255);
            break; //Fin Boton 4

            case 'E': //Boton 5
            delay(500);
            xbyte=5;
            Wire.beginTransmission(8);
            Wire.write(xbyte);
            Wire.endTransmission();
            myGLCD.fillScr(0, 0, 255);
            myGLCD.setColor(255, 0, 0);
            myGLCD.fillRoundRect(80, 70, 239, 169);
            myGLCD.setColor(255, 255, 255);
            myGLCD.setBackColor(255, 0, 0);
            Wire.requestFrom(8, 1);
            dato ="";
            while (Wire.available()) { // slave may send less than requested
              char c = Wire.read(); // receive a byte as character
              Serial.print(c);         // print the character
              dato +=c;
            }
            myGLCD.print(dato, CENTER, 93); //Insertar variable sensada
            myGLCD.print("LUZI 2017", CENTER, 119);
            delay (3000);
            myGLCD.setBackColor(0, 0, 255);
            break; //Fin Boton 5

            case 'F': //Boton 6
            delay(500);
            xbyte=6;
            Wire.beginTransmission(8);
            Wire.write(xbyte);
            Wire.endTransmission();
            myGLCD.fillScr(0, 0, 255);
            myGLCD.setColor(255, 0, 0);
            myGLCD.fillRoundRect(80, 70, 239, 169);
            myGLCD.setColor(255, 255, 255);
            myGLCD.setBackColor(255, 0, 0);
            Wire.requestFrom(8, 3);
            dato ="";
            while (Wire.available()) { // slave may send less than requested
              char c = Wire.read(); // receive a byte as character
              Serial.print(c);         // print the character
              dato +=c;
            }
            myGLCD.print(dato, CENTER, 93); //Insertar variable sensada
            myGLCD.print("LUZI 2017", CENTER, 119);
            delay (3000);
            myGLCD.setBackColor(0, 0, 255);
            break; //Fin Boton 6
            }
           goto bandera; //Regresa al menú
        }
      } 
    }
  } 
 }


