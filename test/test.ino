
const int btnPin = 18;
const int ledPin =  5;


int buttonState = 0;

void setup() {

  pinMode(ledPin, OUTPUT);
  pinMode(btnPin, INPUT_PULLUP);
  Serial.begin(115200);
  Serial.println("HELLO");
}

void loop() {
  // read the state of the pushbutton value:
  buttonState = digitalRead(btnPin);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == LOW) {
    // turn LED on:
    digitalWrite(ledPin, HIGH);
    Serial.println("ON");
  } else {
    // turn LED off:
    digitalWrite(ledPin, LOW);
    Serial.println("OFF");
  }

  delay(1);
}
