#pragma once

#include "Motor.h"

class Truck {
  public:
    void init(byte lEnPin, byte lInaPin, byte lInbPin, byte lPwmPin, byte rEnPin, byte rInaPin, byte rInbPin, byte rPwmPin);
    void go(byte stickVert, byte stickHoriz);
    void goAndTurn(int speed, int turn);
    void speedGo(int speedLeft, int speedRight);
    void autoGo(int speedLeft, int speedRight, int msec);
    void stop();
    void tick();
    bool isRunning();

  private:
    const byte _minStick = 0;
    const byte _maxStick = 255;
    const byte _maxSpeed = 255;
    const byte _maxTurn = 127;

    Motor _left;
    Motor _right;

    unsigned long _tickTime = millis();

    int _targetSpeedLeft;
    int _targetSpeedRight ;
    unsigned long _targetTime ;
    bool _isRunning = false;

    byte filterStickDeadZone(byte value);
};

void Truck::init(byte lEnPin, byte lInaPin, byte lInbPin, byte lPwmPin, byte rEnPin, byte rInaPin, byte rInbPin, byte rPwmPin) {
  _left.init(lEnPin, lInaPin, lInbPin, lPwmPin);
  _right.init(rEnPin, rInaPin, rInbPin, rPwmPin);
}

void Truck::go(byte stickVert, byte stickHoriz) {
  int speed = map(stickVert, _minStick, _maxStick, -_maxSpeed, _maxSpeed);
  int turn = map(stickHoriz, _minStick, _maxStick, -_maxTurn, _maxTurn);

  /*#ifdef DEBUG
    if (millis() - _printTime >= 1000) {
      _printTime = millis();

      Serial.print(speedPos);
      Serial.print("\t");
      Serial.print(speedValue);
      Serial.println();
    }
    #endif*/

  // -_maxSpeed <= speed <= _maxSpeed
  // -_maxTurn <= turn <= _maxTurn

  // speed > 0 & turn = 0 => едем вперед
  // speed = 0 & turn = 0 => стоп
  // speed < 0 & turn = 0 => едем назад

  // speed > 0 & turn > 0 => едем вперед и подворачиваем вправо
  // speed = 0 & turn > 0 => разворот направо на месте
  // speed < 0 & turn > 0 => едем назад и подворачиваем вправо

  // speed > 0 & turn < 0 => едем вперед и подворачиваем влево
  // speed = 0 & turn < 0 => разворот налево на месте
  // speed < 0 & turn < 0 => едем назад и подворачиваем влево

  goAndTurn(speed, turn);
}

void Truck::goAndTurn(int speed, int turn) {
  speed = constrain(speed, -_maxSpeed, _maxSpeed);
  turn = constrain(turn, -_maxTurn, _maxTurn);

  int speedLeft = speed + turn;
  int speedRight = speed - turn;

  /*#ifdef DEBUG
    if (millis() - _printTime >= 1000) {
      _printTime = millis();

      Serial.print(speedLeft);
      Serial.print("\t");
      Serial.print(speedRight);
      Serial.println();
    }
    #endif*/

  speedGo(speedLeft, speedRight);
}

void Truck::speedGo(int speedLeft, int speedRight) {
  _left.go(speedLeft);
  _right.go(speedRight);
}

void Truck::autoGo(int speedLeft, int speedRight, int msec) {
  _targetSpeedLeft = speedLeft;
  _targetSpeedRight = speedRight;
  _targetTime = millis() + msec;
  _isRunning = true;
}

void Truck::stop() {
  _left.stop();
  _right.stop();
  _isRunning = false;
}

void Truck::tick() {
  if (!_isRunning) {
    return;
  }

  if (millis() > _targetTime) {
    stop();
    return;
  }

  speedGo(_targetSpeedLeft, _targetSpeedRight);
}

bool Truck::isRunning() {
  return _isRunning;
}

byte Truck::filterStickDeadZone(byte value) {
  if (value > 100 && value < 156)
    return 128;
  else
    return value;
}
