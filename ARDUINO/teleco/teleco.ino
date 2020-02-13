
#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
#include "Adafruit_MCP23017.h"

/*
  PRO MICRO -- PRECAUTIONS

  • Installer les boards SparkFun :
  - add URL de gestionnaire de cartes supplémentaires "https://raw.githubusercontent.com/sparkfun/Arduino_Boards/master/IDE_Board_Manager/package_sparkfun_index.json"
  - Tools/Type de Carte/Gestionnaire de Carte/ Installer "SparkFun AVR Boards"

  • Arduino IDE Burning Options
  - Type de carte "sparkfun Pro Micro"
  - Processeur "ATMega 32U4 (5V,16Mhz)"
  - Port : "..."

  • Si le pro micro a été gravé avec la mauvaise config, il est possible qu'il soit pété et pas re-gravable.
    FIX: Re-graver le bootloader depuis un arduino Uno. https://eldontronics.wordpress.com/2019/06/16/arduino-pro-micro-clone-port-not-detected/
  - Connecter Arduino Uno (Pins 5V, GND, 13, 12, 11, 10) <--> Pro Micro (Pins 5V, GND, 15, 14, 16, RST)
  - Graver le sketch exemple ArduinoISP sur le Arduino Uno
  - Se mettre dans la config
    type de carte "sparkfun Pro Micro" / Processeur "ATMega 32U4 (5V,16Mhz)" / Port : "celui de l'arduino UNO" / Programmateur: "Arduino as ISP"
    et "Graver la séquence d'initialisation"
  - Ré-essayer de graver directement le Pro Micro en conditions normales.

*/

Adafruit_MCP23017 mcp;
U8G2_SSD1309_128X64_NONAME0_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 15, /* data=*/ 16, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);

// CONFIG
int Brightness = 20;
int longPressDelay = 500;
int holdRepeatDelay = 100;
// PINOUT
int switches[8]= {8,10,12,14,0,2,4,6};
int leds[8]= {9,11,13,15,1,3,5,7};
String switches_functions[8]= {"A","B","C","D","X","Y","UP","DOWN"}; // {"play","pause","prev","next","mute","func","up","down"};

// UTILS
int switches_states[8]= {0,0,0,0,0,0,0,0};
unsigned long switches_times[8]= {0,0,0,0,0,0,0,0};
unsigned long Tnow = 0;


void setup() {

  Serial.begin(115200);
  Serial.setTimeout(10);

  // SCREEN
  u8g2.begin();
  u8g2.enableUTF8Print();

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
        switches_times[i]=Tnow-longPressDelay+holdRepeatDelay;
        longPress(i);
      }
    }
    // OFF
    if(mcp.digitalRead(switches[i])&&(switches_states[i]!=0)){
      switches_states[i]=0;
      releasePress(i);
    }
  }


  // SCREEN START
  // u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x12_me);
  readAndDisplay();

  // TYPO TEST
  // u8g2.drawStr(0,10,"Hello KXKM World! p");
  // u8g2.drawStr(0,22,"I'm the raspi-remote");
  // u8g2.drawStr(0,34,"Yes p");
  // u8g2.drawStr(0,46,"Whats up");
  // u8g2.drawStr(0,58,"I don't know •••• ••••");

  // UTILS
  u8g2.sendBuffer();
  delay(1);


  }

  void simplePress(int i){
    String rpi_com = switches_functions[i]+"-down";
    Serial.println(rpi_com);
    u8g2.drawStr(0,58,(rpi_com+"                   ").c_str());
  }

  void longPress(int i){
    String rpi_com = switches_functions[i]+"-hold";
    Serial.println(rpi_com);
    u8g2.drawStr(0,58,(rpi_com+"                     ").c_str());
  }

  void releasePress(int i){
    String rpi_com = switches_functions[i]+"-up";
    Serial.println(rpi_com);
    u8g2.drawStr(0,58,(rpi_com+"                     ").c_str());
  }

  void readAndDisplay(){
    String input = Serial.readString();
    if(input.length()!=0){
      // Add white space at end of txt
      input = input.substring(0, input.length() - 1);
      String long_end = "                       ";
      input+=long_end;
      // Get args
      int input_arg1 = atoi(&input[0]);
      int input_arg2 = atoi(&input[2]);
      // STANDARD TEXT ( Line 1--5 )
      if((input_arg1>=1)&&(input_arg1<=5)){
        // txt style
        if(input_arg2==1){u8g2.setDrawColor(0);}else u8g2.setDrawColor(1);
        // remove args
        input.remove(0,4);
        int posY = input_arg1*10+(input_arg1-1)*2;
        u8g2.drawStr(0,posY,input.c_str());
        u8g2.sendBuffer();
      }
      // SPECIAL ARGS
      if(input_arg1==0){
        u8g2.clearBuffer();
      }
    }
  }
