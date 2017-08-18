// Touch screen library with X Y and Z (pressure) readings as well
// as oversampling to avoid 'bouncing'
// This demo code returns raw readings, public domain

#include <stdint.h>
#include "TouchScreen.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <SWTFT.h> // Hardware-specific library
#include <TouchScreen.h>
#include <Wire.h>


// These are the pins for the shield!
#define YP A1  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 7   // can be a digital pin
#define XP 6   // can be a digital pin

#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940

//TouchScreen ts = TouchScreen(XP, YP, XM, YM, 600);

#define MINPRESSURE 10
#define MAXPRESSURE 1000

#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

SWTFT tft;

uint8_t rotation=3;

void setup(void) {
  Wire.begin();
  Serial.begin(9600);
  tft.reset();
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.setRotation(rotation);
  tft.fillScreen(BLACK);
  delay(10);
  testText();
}

byte x = 0;

void loop(void) {
  // a point object holds x y and z coordinates
  TSPoint p = ts.getPoint();
  
  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
     Serial.print("X = "); Serial.print(p.x);
     Serial.print("\tY = "); Serial.print(p.y);
     Serial.print("\tPressure = "); Serial.println(p.z);
     if (p.y>831){
      llamado("Ritmo Cardiaco");
        //testText();
        //delay(2000);
        //tft.fillScreen(WHITE);
        //delay(10);
        delay(1000);
     }
     if (p.y>701 && p.y<830){
      llamado("Frecuencia Respiratoria");
      delay(1000);
     }
     if (p.y>601 && p.y<700){
      llamado("Frecuencia Cardiaca Fetal");
      delay(1000);
     }
     if (p.y>500 && p.y<600){
      llamado("Temperatura");
      delay(1000);
     }
     if (p.y>400 && p.y<499){
      llamado("Presion Arterial");
      delay(1000);
     }
     if (p.y>200 && p.y<390){
      llamado("Glusosa");
      delay(1000);
     }
     if (p.y<200){
      llamado("Contracciones");
      delay(1000);
     }
   }
}


unsigned long testText() {
  tft.setCursor(0,0);
  tft.setTextColor(RED);  tft.setTextSize(2);
  tft.println("Ritmo cardiaco");
  tft.setCursor(0,30);
  tft.setTextColor(RED); tft.setTextSize(2);
  tft.println("Frecuencia respiratoria");
  tft.setCursor(0,70);
  tft.setTextColor(RED);    tft.setTextSize(2);
  tft.println("Frecuencia Cardiaca Fetal");
  tft.setCursor(0,110);
  tft.setTextColor(RED);    tft.setTextSize(2);
  tft.println("Temperatura");
  tft.setCursor(0,150);
  tft.setTextColor(RED);    tft.setTextSize(2);
  tft.println("Presion Arterial");
  tft.setCursor(0,190);
  tft.setTextColor(RED);    tft.setTextSize(2);
  tft.println("Glucosa");
  tft.setCursor(0,230);
  tft.setTextColor(RED);    tft.setTextSize(2);
  tft.println("Contracciones");
}

void llamado(String data){
  Wire.beginTransmission(8); /
  Wire.write(data);          
  Wire.endTransmission();    
  delay(500);
}