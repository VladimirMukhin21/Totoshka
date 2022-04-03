#define PIN_VRX1 A0
#define PIN_VRY1 A1
#define PIN_BUTTON1 3

#define PIN_VRX2 A2
#define PIN_VRY2 A3
#define PIN_BUTTON2 4

void setup () {
  Serial.begin (9600);
  pinMode(PIN_BUTTON1, INPUT_PULLUP);
  pinMode(PIN_BUTTON2, INPUT_PULLUP);
}

void loop () {
  Serial.print("X = ");
  Serial.print(analogRead(PIN_VRX1));

  Serial.print("\tY = ");
  Serial.print(analogRead(PIN_VRY1));

  Serial.print("\tbutton = ");
  if (digitalRead(PIN_BUTTON1) == HIGH) {
    Serial.print ("NOT CLICK");
  }
  else {
    Serial.print ("CLICK!");
  }

  Serial.print("\t\tX = ");
  Serial.print(analogRead(PIN_VRX2));

  Serial.print("\tY = ");
  Serial.print(analogRead(PIN_VRY2));

  Serial.print("\tbutton = ");
  if (digitalRead(PIN_BUTTON2) == HIGH) {
    Serial.println ("NOT CLICK");
  }
  else {
    Serial.println ("CLICK!");
  }
  delay(1000);
}
