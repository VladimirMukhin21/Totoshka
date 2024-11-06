#include <EncButton.h>
#include "Radio.h"
#include "Keyboard.h"
#include "Command.h"

#define PIN_LEFT_STICK_VERT A13
#define PIN_LEFT_STICK_HORIZ A15
#define PIN_LEFT_STICK_BUTTON 33

#define PIN_RIGHT_STICK_VERT A12
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
EncButton<EB_TICK, PIN_FRONT_BLACK_BUTTON> blackButton;
byte blackButtonPos = 0;
Keyboard keyboard;
Command command;
bool switchTelemetry = false;

byte speed = 127;
byte turn = 127;
unsigned long timeOfReadDrive = millis();

void setup() {
  Serial.begin(115200);
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
  blackButton.tick();

  Payload payload;

  payload.leftStick.vert = map(analogRead(PIN_LEFT_STICK_VERT), 0, 1023, 255, 0);
  payload.leftStick.horiz = map(analogRead(PIN_LEFT_STICK_HORIZ), 0, 1023, 0, 255);
  payload.leftStick.pressed = !digitalRead(PIN_LEFT_STICK_BUTTON);

  payload.rightStick.vert = map(analogRead(PIN_RIGHT_STICK_VERT), 0, 1023, 255, 0);
  payload.rightStick.horiz = map(analogRead(PIN_RIGHT_STICK_HORIZ), 0, 1023, 0, 255);
  payload.rightStick.pressed = !digitalRead(PIN_RIGHT_STICK_BUTTON);

  payload.frontSwitch = mapSwitch3Pos(!digitalRead(PIN_FRONT_SWITCH_UP), !digitalRead(PIN_FRONT_SWITCH_DOWN));

  payload.frontRedButton = !digitalRead(PIN_FRONT_RED_BUTTON);
  payload.frontYellowButton = !digitalRead(PIN_FRONT_YELLOW_BUTTON);
  payload.frontWhiteButton = !digitalRead(PIN_FRONT_WHITE_BUTTON);
  payload.frontBlackButtonSwitch = mapButtonSwitch(blackButton.press(), blackButtonPos, 2);

  payload.upBlueButton = !digitalRead(PIN_UP_BLUE_BUTTON);
  payload.upGreenButton = !digitalRead(PIN_UP_GREEN_BUTTON);

  if (payload.upBlueButton || payload.frontWhiteButton) {
    payload.key = keyboard.getKey();
  }

  if (Serial.available()) {
    command.readPart();
    if (command.isCompleted()) {
      if (command.command == command.TELEMETRY_START) {
        switchTelemetry = true;
      }
      else if (command.command == command.TELEMETRY_STOP) {
        switchTelemetry = false;
      }
      else if (command.command == command.DRIVE) {
        speed = command.args[0];
        turn = command.args[1];
        timeOfReadDrive = millis();
      }

      command.clear(); // команду исполнили, очищаем
    }
  }

  if (millis() - timeOfReadDrive < 200) {
    // первые 200мс после загрузки пульта робот не будет ехать из-за условия выше
    payload.rightStick.vert = speed;
    payload.rightStick.horiz = turn;
    payload.rightStick.pressed = false;
  }

  payload.switchTelemetry = switchTelemetry;

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
