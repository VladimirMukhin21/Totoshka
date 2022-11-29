#include <EncButton.h>
#include "Radio.h"

#define PIN_LEFT_STICK_VERT  A13
#define PIN_LEFT_STICK_HORIZ A15
#define PIN_LEFT_STICK_BUTTON 33

#define PIN_RIGHT_STICK_VERT  A12
#define PIN_RIGHT_STICK_HORIZ A14
#define PIN_RIGHT_STICK_BUTTON 32

#define PIN_FRONT_SWITCH_DOWN 24
#define PIN_FRONT_SWITCH_UP 25

#define PIN_FRONT_RED_BUTTON 26
#define PIN_FRONT_YELLOW_BUTTON 27
#define PIN_FRONT_WHITE_BUTTON 28
#define PIN_FRONT_BLACK_BUTTON 29

#define PIN_UP_BLUE_BUTTON 30
#define PIN_UP_GREEN_BUTTON 31

Radio radio;

EncButton<EB_TICK, PIN_FRONT_RED_BUTTON> redButton;
EncButton<EB_TICK, PIN_FRONT_YELLOW_BUTTON> yellowButton;
EncButton<EB_TICK, PIN_FRONT_WHITE_BUTTON> whiteButton;
EncButton<EB_TICK, PIN_FRONT_BLACK_BUTTON> blackButton;

EncButton<EB_TICK, PIN_UP_GREEN_BUTTON> greenButton;

byte blackButtonPos = 0;

void setup() {
  Serial.begin(9600);
  radio.initTransmitter();
  pinMode(PIN_LEFT_STICK_BUTTON, INPUT_PULLUP);
  pinMode(PIN_RIGHT_STICK_BUTTON, INPUT_PULLUP);

  pinMode(PIN_UP_BLUE_BUTTON, INPUT_PULLUP);
  pinMode(PIN_UP_GREEN_BUTTON, INPUT_PULLUP);

  pinMode(PIN_FRONT_RED_BUTTON, INPUT_PULLUP);
  pinMode(PIN_FRONT_YELLOW_BUTTON, INPUT_PULLUP);
  pinMode(PIN_FRONT_WHITE_BUTTON, INPUT_PULLUP);
  //pinMode(PIN_FRONT_BLACK_BUTTON, INPUT_PULLUP);

  pinMode(PIN_FRONT_SWITCH_DOWN, INPUT_PULLUP);
  pinMode(PIN_FRONT_SWITCH_UP, INPUT_PULLUP);
}

void loop() {
  redButton.tick();
  yellowButton.tick();
  whiteButton.tick();
  blackButton.tick();
  
  greenButton.tick();
  
  Payload payload;

  payload.leftStick.vert   = map(analogRead(PIN_LEFT_STICK_VERT), 0, 1023, 255, 0);
  payload.leftStick.horiz  = map(analogRead(PIN_LEFT_STICK_HORIZ), 0, 1023, 0, 255);
  payload.leftStick.pressed = !digitalRead(PIN_LEFT_STICK_BUTTON);

  payload.rightStick.vert  = map(analogRead(PIN_RIGHT_STICK_VERT), 0, 1023, 255, 0);
  payload.rightStick.horiz = map(analogRead(PIN_RIGHT_STICK_HORIZ), 0, 1023, 0, 255);
  payload.rightStick.pressed = !digitalRead(PIN_RIGHT_STICK_BUTTON);

  payload.frontSwitch = mapSwitch3Pos(!digitalRead(PIN_FRONT_SWITCH_UP), !digitalRead(PIN_FRONT_SWITCH_DOWN));

  payload.frontRedButton = redButton.press();
  payload.frontYellowButton = yellowButton.press();
  payload.frontWhiteButton = whiteButton.press();
  payload.frontBlackButtonSwitch = mapButtonSwitch(blackButton.press(), blackButtonPos, 2);

  payload.upBlueButton = !digitalRead(PIN_UP_BLUE_BUTTON);
  payload.upGreenButton = greenButton.press();

  radio.write(payload);
}

byte mapSwitch3Pos(bool up, bool down) {
  return up ? 0 : (down ? 2 : 1);
}

byte mapButtonSwitch(bool fired, byte& currentPos, byte posCount) {
  if (fired) {
    currentPos++;
  }
  
  return currentPos % posCount;
}
