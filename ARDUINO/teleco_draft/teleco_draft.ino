

#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>

#include <Wire.h>
#include "Adafruit_MCP23017.h"

Adafruit_MCP23017 mcp;


U8G2_SSD1309_128X64_NONAME0_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 15, /* data=*/ 16, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);


void setup() {

  // Serial.begin(115200);

  // SCREEN
  u8g2.begin();

  // MCP
  mcp.begin(); // default address 0

  // BTNS
  // INPUT_PULLUP
  mcp.pinMode(0, INPUT);
  mcp.pullUp(0, HIGH);
  mcp.pinMode(1, INPUT);
  mcp.pullUp(1, HIGH);
  mcp.pinMode(2, INPUT);
  mcp.pullUp(2, HIGH);
  mcp.pinMode(3, INPUT);
  mcp.pullUp(3, HIGH);

  // LEDS
  // MCP_PIN --- (-) [LED] (+) --- PWM PIN (the pwm pin sets the brightness level)
  mcp.pinMode(4, OUTPUT);
  mcp.pinMode(5, OUTPUT);
  mcp.pinMode(6, OUTPUT);
  mcp.pinMode(7, OUTPUT);
  // Level Setter
  pinMode(6, OUTPUT);
  analogWrite(6, 10);

  }

void loop() {

  mcp.digitalWrite(4, !mcp.digitalRead(0));
  mcp.digitalWrite(5, !mcp.digitalRead(1));
  mcp.digitalWrite(6, !mcp.digitalRead(2));
  mcp.digitalWrite(7, !mcp.digitalRead(3));


  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x12_mr);

  u8g2.drawStr(0,8,"Hello teleco !");
  if(!mcp.digitalRead(0)){
    u8g2.drawStr(0,20,"btn1 pressed");
  }
  if(!mcp.digitalRead(1)){
    u8g2.drawStr(0,32,"btn2 pressed");
  }
  if(!mcp.digitalRead(2)){
    u8g2.drawStr(0,44,"btn3 pressed");
  }
  if(!mcp.digitalRead(3)){
    u8g2.drawStr(0,56,"btn4 pressed");
  }

  // u8g2.drawStr(0,8,"Hello KXKM World!");
  // u8g2.drawStr(0,20,"I'm the raspi-remote");
  // u8g2.drawStr(0,32,"yes");
  // u8g2.drawStr(0,44,"whats up");
  // u8g2.drawStr(0,56,"I don't know");

  u8g2.sendBuffer();
  delay(1);


  }
