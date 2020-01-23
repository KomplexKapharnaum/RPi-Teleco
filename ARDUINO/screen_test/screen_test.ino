
#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>


U8G2_SSD1309_128X64_NONAME0_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 15, /* data=*/ 16, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);


// 3.12" test -- Memoire insuffisante sur le pro micro
// U8G2_SSD1322_NHD_256X64_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);


void setup(void) {
  u8g2.begin();
}

void loop(void) {
  u8g2.clearBuffer();					// clear the internal memory
  u8g2.setFont(u8g2_font_6x12_mr);	// choose font // https://github.com/olikraus/u8g2/wiki/fntlistall
  u8g2.drawStr(0,8,"Hello KXKM World!");
  u8g2.drawStr(0,20,"I'm the raspi-remote");
  u8g2.drawStr(0,32,"yes");
  u8g2.drawStr(0,44,"whats up");
  u8g2.drawStr(0,56,"I don't know");
  u8g2.sendBuffer();					// transfer internal memory to the display
  delay(1000);
}
