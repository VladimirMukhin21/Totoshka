void setup() {
  for (int pin = 0; pin<= 53; pin++) pinMode(pin, OUTPUT);
  Serial.begin(9600);
}

// the loop function runs over and over again forever
void loop() {
  for (int pin = 0; pin<= 53; pin++) digitalWrite(pin, HIGH);
  delay(250);
  for (int pin = 0; pin<= 53; pin++) digitalWrite(pin, LOW);
  delay(250);

  for (int pin = 0; pin<= 53; pin++) digitalWrite(pin, HIGH);
  delay(250);
  for (int pin = 0; pin<= 53; pin++) digitalWrite(pin, LOW);
  delay(1000);

  for (int pin = A0; pin<= A15; pin++) {
    Serial.print(analogRead(pin));
    Serial.print('\t');
  }
  Serial.println();
}
