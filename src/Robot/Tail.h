#pragma once

#include <Servo.h>
#include "Angle.h"

class Tail {
public:
  void init(byte coccyxPin);
  void operate(byte stickVert);
  void moveTo(int deg);
  void upTail();
  void stop();
  void tick();
  bool isRunning();

private:
  const byte _minStick = 0;
  const byte _maxStick = 255;
  const byte _maxSpeed = 5;

  Servo _coccyx;

  Angle _coccyxAngle = Angle(145, 15, 145, 60); // вниз-вверх; макс вверх для больших колес - 140, для маленьких - 145

  //#define DEBUG
#ifdef DEBUG
  unsigned long _printTime = millis();
#endif

  unsigned long _tickTime = millis();
  int _targetDeg;
  bool _isRunning = false;

  byte filterStickDeadZone(byte value);
};

void Tail::init(byte coccyxPin) {
  _coccyx.attach(coccyxPin);
  _coccyx.write(_coccyxAngle.toDeg());
}

void Tail::operate(byte stickVert) {
  int speedVert = map(filterStickDeadZone(stickVert), _minStick, _maxStick + 1, -_maxSpeed, _maxSpeed + 1);
  if (speedVert != 0) {
    _isRunning = false;

    _coccyxAngle.addPoints(speedVert);
    _coccyx.write(_coccyxAngle.toDeg());
  }

  /*#ifdef DEBUG
  if (millis() - _printTime >= 1000) {
    _printTime = millis();
    Serial.print("stick = ");
    Serial.print(stickVert);
    Serial.print("\tangle = ");
    Serial.print(_coccyxAngle.toDeg());
    Serial.print("\tspeed = ");
    Serial.print(speedVert);
    Serial.println();
  }
#endif*/
}

void Tail::moveTo(int deg) {
  _targetDeg = deg;
  _isRunning = true;
}

void Tail::upTail() {
  moveTo(150);
}

void Tail::stop() {
  _isRunning = false;
}

void Tail::tick() {
  if (!_isRunning) {
    return;
  }

  if (millis() - _tickTime >= 1) {
    _tickTime = millis();

    int delta = _coccyxAngle.toDeg() - _targetDeg;
    if (delta > 1) {
      _coccyxAngle.addPoints(-5);
      _coccyx.write(_coccyxAngle.toDeg());
    }
    else if (delta < -1) {
      _coccyxAngle.addPoints(5);
      _coccyx.write(_coccyxAngle.toDeg());
    }
    else {
      _isRunning = false;
    }
  }
}

bool Tail::isRunning() {
  return _isRunning;
}

byte Tail::filterStickDeadZone(byte value) {
  if (value > 88 && value < 168)
    return 128;
  else
    return value;
}
