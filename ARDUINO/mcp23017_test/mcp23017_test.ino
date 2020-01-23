#include <Wire.h>
#include "Adafruit_MCP23017.h"

// Basic pin reading and pullup test for the MCP23017 I/O expander
// public domain!

// Connect pin #12 of the expander to Analog 5 (i2c clock)
// Connect pin #13 of the expander to Analog 4 (i2c data)
// Connect pins #15, 16 and 17 of the expander to ground (address selection)
// Connect pin #9 of the expander to 5V (power)
// Connect pin #10 of the expander to ground (common ground)
// Connect pin #18 through a ~10kohm resistor to 5V (reset pin, active low)

// Input #0 is on pin 21 so connect a button or switch from there to ground

Adafruit_MCP23017 mcp;

void setup() {

  // Serial.begin(115200);


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
  analogWrite(6, 100);

  }

void loop() {

  // Serial.print(" btn1 ");Serial.print(mcp.digitalRead(0));
  // Serial.print(" btn2 ");Serial.print(mcp.digitalRead(1));
  // Serial.print(" btn3 ");Serial.print(mcp.digitalRead(2));
  // Serial.print(" btn4 ");Serial.println(mcp.digitalRead(3));

  mcp.digitalWrite(4, mcp.digitalRead(0));
  mcp.digitalWrite(5, mcp.digitalRead(1));
  mcp.digitalWrite(6, mcp.digitalRead(2));
  mcp.digitalWrite(7, mcp.digitalRead(3));



  }
