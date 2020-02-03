
#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
#include "Adafruit_MCP23017.h"

Adafruit_MCP23017 mcp;
U8G2_SSD1309_128X64_NONAME0_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 15, /* data=*/ 16, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);

// CONFIG
int Brightness = 20;
int longPressDelay = 500;
// PINOUT
int switches[8]= {8,10,12,14,0,2,4,6};
int leds[8]= {9,11,13,15,1,3,5,7};
String switches_functions[8]= {"pause","play","prev","next","mute","func","up","down"};

// UTILS
int switches_states[8]= {0,0,0,0,0,0,0,0};
unsigned long switches_times[8]= {0,0,0,0,0,0,0,0};
unsigned long Tnow = 0;


void setup() {

  Serial.begin(115200);

  // SCREEN
  u8g2.begin();

  // MCP
  mcp.begin(); // default address 0

  // BTNS
  // INPUT_PULLUP
  mcp.pinMode(switches[0], INPUT); mcp.pullUp(switches[0], HIGH);
  mcp.pinMode(switches[1], INPUT); mcp.pullUp(switches[1], HIGH);
  mcp.pinMode(switches[2], INPUT); mcp.pullUp(switches[2], HIGH);
  mcp.pinMode(switches[3], INPUT); mcp.pullUp(switches[3], HIGH);
  mcp.pinMode(switches[4], INPUT); mcp.pullUp(switches[4], HIGH);
  mcp.pinMode(switches[5], INPUT); mcp.pullUp(switches[5], HIGH);
  mcp.pinMode(switches[6], INPUT); mcp.pullUp(switches[6], HIGH);
  mcp.pinMode(switches[7], INPUT); mcp.pullUp(switches[7], HIGH);
  // LEDS
  // MCP_PIN --- (-) [LED] (+) --- PWM PIN (the pwm pin sets the brightness level)
  mcp.pinMode(leds[0], OUTPUT);
  mcp.pinMode(leds[1], OUTPUT);
  mcp.pinMode(leds[2], OUTPUT);
  mcp.pinMode(leds[3], OUTPUT);
  mcp.pinMode(leds[4], OUTPUT);
  mcp.pinMode(leds[5], OUTPUT);
  mcp.pinMode(leds[6], OUTPUT);
  mcp.pinMode(leds[7], OUTPUT);
  // Level Setter
  pinMode(6, OUTPUT);
  analogWrite(6, Brightness);

  }

void loop() {

  Tnow = millis();

  // LED FEEDBACK
  mcp.digitalWrite(1, mcp.digitalRead(0));
  mcp.digitalWrite(3, mcp.digitalRead(2));
  mcp.digitalWrite(5, mcp.digitalRead(4));
  mcp.digitalWrite(7, mcp.digitalRead(6));
  mcp.digitalWrite(9, mcp.digitalRead(8));
  mcp.digitalWrite(11, mcp.digitalRead(10));
  mcp.digitalWrite(13, mcp.digitalRead(12));
  mcp.digitalWrite(15, mcp.digitalRead(14));

  // SCREEN START
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x12_mr);
  u8g2.drawStr(0,8,"btn pressed:");

  // GET BTNS
  for (int i = 0; i < 8; i++) {
    // ON
    if(!mcp.digitalRead(switches[i])){
      // simple
      if(switches_states[i]==0){
        switches_states[i]=1;
        switches_times[i]=Tnow;
        simplePress(i);
      }
      // long
      if((switches_states[i]==1)&&(Tnow-switches_times[i]>longPressDelay)){
        longPress(i);
      }
    }
    // OFF
    if(mcp.digitalRead(switches[i])&&(switches_states[i]!=0)){
      switches_states[i]=0;
    }
  }

  // TYPO TEST
  // u8g2.drawStr(0,8,"Hello KXKM World!");
  // u8g2.drawStr(0,20,"I'm the raspi-remote");
  // u8g2.drawStr(0,32,"yes");
  // u8g2.drawStr(0,44,"whats up");
  // u8g2.drawStr(0,56,"I don't know");

  // UTILS
  u8g2.sendBuffer();
  delay(1);


  }

  void simplePress(int i){
    // String rpi_com = "/simple_press "+String(i);
    String rpi_com = switches_functions[i]+" simple_press ";
    Serial.println(rpi_com);
    char cstr[16];
    itoa(i, cstr, 10);
    u8g2.drawStr(0,20,cstr);
  }

  void longPress(int i){
    // String rpi_com = "/long_press "+String(i);
    String rpi_com = switches_functions[i]+" long_press ";
    Serial.println(rpi_com);
    char cstr[16];
    itoa(i, cstr, 10);
    u8g2.drawStr(0,20,cstr);
  }
