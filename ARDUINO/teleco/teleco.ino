
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
int Brightness = 1;
int longPressDelay = 400;
int holdRepeatDelay = 100;

// PINOUT
int switches[8] = {8, 10, 12, 14, 0, 2, 4, 6};
int leds[8] = {9, 11, 13, 15, 1, 3, 5, 7};
String s_fn[8] = {"A", "B", "C", "D", "MUTE", "FUNC", "UP", "DOWN"}; // {"play","pause","prev","next","mute","func","up","down"};

// UTILS
int switches_states[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned long switches_times[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned long Tnow = 0;

// RECV
const byte numChars = 32;
char receivedChars[numChars];
boolean newData = false;


void drawStringWithSymbol( int x, int y, String s) {

  bool special = false;
  bool unifont = false;
  bool colored = false;

  // clear line
  u8g2.drawUTF8(x, y, "                      ");
  
  for (int k = 0; k < s.length(); k++) {

    // engage special
    if (s[k] == '^') special = true;
    
    // special engaged
    else if (special) {
      if (s[k] == '0') u8g2.setDrawColor(1);      // Black BG
      else if (s[k] == '1') u8g2.setDrawColor(0); // White BG
      else if (s[k] == '2') u8g2.setFont(u8g2_font_open_iconic_embedded_1x_t);
      else if (s[k] == '3') u8g2.setFont(u8g2_font_open_iconic_gui_1x_t);
      else if (s[k] == '4') u8g2.setFont(u8g2_font_open_iconic_thing_1x_t);
      else if (s[k] == '5') u8g2.setFont(u8g2_font_open_iconic_play_1x_t);
      else if (s[k] == '6') u8g2.setFont(u8g2_font_open_iconic_arrow_1x_t);
      else if (s[k] == '7') u8g2.setFont(u8g2_font_open_iconic_human_1x_t);
      else if (s[k] == '8') u8g2.setFont(u8g2_font_open_iconic_mime_1x_t);
      else if (s[k] == '9') u8g2.setFont(u8g2_font_open_iconic_www_1x_t);
      
      if (s[k] >= '2') unifont = true;
      special = false;
    }
    
    // draw next character
    else {
      x += u8g2.drawUTF8(x, y, String(s[k]).c_str() );
      if (unifont) {
        x += 5;
        u8g2.setFont(u8g2_font_6x12_me);
        unifont = false;
      }
    }
    
  }
  
  u8g2.setDrawColor(1);
  
}

void setup() {

  Serial.begin(115200);
  delay(2000);
  Serial.setTimeout(10);

  // SCREEN
  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.clearBuffer();
  
  u8g2.setFont(u8g2_font_6x12_me);
  u8g2.setFontMode(0);
  u8g2.setDrawColor(1);
  //drawStringWithSymbol(0, 9, "^1  MEDIA 0123456789\"              ");
  //drawStringWithSymbol(0, 35, "^1    ^0 starting.. ^7B^1     ");
  drawStringWithSymbol(0, 35, "    ^0 starting.. ^7B     ");
  //drawStringWithSymbol(0, 12*5+2, "^1      dp                ");
  u8g2.sendBuffer();

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
  mcp.digitalWrite(1, !mcp.digitalRead(0));
  mcp.digitalWrite(3, !mcp.digitalRead(2));
  mcp.digitalWrite(5, !mcp.digitalRead(4));
  mcp.digitalWrite(7, !mcp.digitalRead(6));
  mcp.digitalWrite(9, !mcp.digitalRead(8));
  mcp.digitalWrite(11, !mcp.digitalRead(10));
  mcp.digitalWrite(13, !mcp.digitalRead(12));
  mcp.digitalWrite(15, !mcp.digitalRead(14));


  // GET BTNS
  for (int i = 0; i < 8; i++) {
    
    // ON
    if (!mcp.digitalRead(switches[i])) {
      // simple
      if (switches_states[i] == 0) {
        Serial.println(s_fn[i] + "-down");
        switches_times[i] = Tnow;
        switches_states[i] = 1;
      }
      // long
      if ((switches_states[i] > 0) && (Tnow - switches_times[i] > longPressDelay)) {
        Serial.println(s_fn[i] + "-hold");
        switches_times[i] = Tnow - longPressDelay + holdRepeatDelay;
        switches_states[i] = 2;
      }
    }
    
    // OFF
    if (mcp.digitalRead(switches[i]) && (switches_states[i] != 0))  {
      if (switches_states[i] == 1) Serial.println(s_fn[i] + "-up");
      else if (switches_states[i] == 2) Serial.println(s_fn[i] + "-holdup");
      switches_states[i] = 0;
    }
  }


  // SCREEN START
  recvWithStartEndMarkers();

}


void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char fluxMarker = '¤';
  char splitMarker = '£';
  char rc;
  bool dirty = false;

  while (Serial.available() > 0 && newData == false) {
    
    rc = Serial.read();
    
    if (recvInProgress == true) {

      // End of Line
      if (rc == fluxMarker || rc == splitMarker) {
        receivedChars[ndx] = '\0'; // terminate the string
        ndx = 0;
        dirty = true;

        // Draw
        String input = String(receivedChars);
        if (input.length() != 0) {
          // Get args
          int input_arg1 = atoi(&input[0]);
      
          // SPECIAL ARGS
          if (input_arg1 == 0) u8g2.clearBuffer();
      
          // STANDARD TEXT ( Line 1--5 )
          else if ((input_arg1 >= 1) && (input_arg1 <= 5)) {
            input.remove(0, 1);
            int posY = 10 + 13*(input_arg1-1);
            drawStringWithSymbol(0, posY, input);
          }
        }

        // End of Transmission
        if (rc == fluxMarker) recvInProgress = false;
      }

      // Receive in progress
      else {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) ndx = numChars - 1;
      }
      
    }

    else if (rc == fluxMarker) {
      recvInProgress = true;
      ndx = 0;
    }
  }

  // Push to screen
  if (dirty) u8g2.sendBuffer();
  
}
