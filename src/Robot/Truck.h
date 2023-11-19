#pragma once

#include "Motor.h"
#include "Gyro.h"

class Truck {
public:
  void init(byte lEnPin, byte lInaPin, byte lInbPin, byte lPwmPin, byte rEnPin, byte rInaPin, byte rInbPin, byte rPwmPin, Gyro &gyro);
  void operate(byte stickVert, byte stickHoriz);
  void goAndTurn(int speed, int turn);
  void speedGo(int speedLeft, int speedRight);
  void autoGo(int speed, int msec = -1);
  void goStraight(int speed, int msec = -1);
  void stop();
  void tick();
  bool isRunning();

private:
  enum Mode {
    NONE,
    AUTO_GO,
    GO_STRAIGHT
  };

  const byte _minStick = 0;
  const byte _maxStick = 255;
  const byte _maxSpeed = 255;
  const byte _maxTurn = 255;

  Motor _left;
  Motor _right;
  Gyro *_gyro;
  long _deviation = 0;

  /*#ifdef DEBUG
      unsigned long _printTime = millis();
      #endif*/

  unsigned long _tickTime = millis();

  int _targetSpeed;
  unsigned long _targetTime;
  Mode _mode = NONE;

  byte filterStickDeadZone(byte value);
};

void Truck::init(byte lEnPin, byte lInaPin, byte lInbPin, byte lPwmPin, byte rEnPin, byte rInaPin, byte rInbPin, byte rPwmPin, Gyro &gyro) {
  _left.init(lEnPin, lInaPin, lInbPin, lPwmPin);
  _right.init(rEnPin, rInaPin, rInbPin, rPwmPin);
  _gyro = &gyro;
}

void Truck::operate(byte stickVert, byte stickHoriz) {
  stickVert = filterStickDeadZone(stickVert);
  stickHoriz = filterStickDeadZone(stickHoriz);
  int speed = map(stickVert, _minStick, _maxStick + 1, -_maxSpeed, _maxSpeed + 1);
  int turn = map(stickHoriz, _minStick, _maxStick + 1, -_maxTurn, _maxTurn + 1);

  /*#ifdef DEBUG
    if (millis() - _printTime >= 1000) {
    _printTime = millis();

    Serial.print(speed);
    Serial.print("\t");
    Serial.print(turn);
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

  if (_mode == NONE) {
    goAndTurn(speed, turn);
  } else {
    if (speed != 0 || turn != 0) {
      goAndTurn(speed, turn);
      _mode = NONE;
    }
  }
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

void Truck::autoGo(int speed, int msec = -1) {
  _targetSpeed = speed;
  _targetTime = -1;
  if (msec > 0) {
    _targetTime = millis() + msec;
  }

  _mode = AUTO_GO;
}

void Truck::goStraight(int speed, int msec = -1) {
  _targetSpeed = speed;
  _targetTime = -1;
  if (msec > 0) {
    _targetTime = millis() + msec;
  }

  _deviation = 0;
  _mode = GO_STRAIGHT;
}

void Truck::stop() {
  _left.stop();
  _right.stop();
  _mode = NONE;
}

void Truck::tick() {
  if (_mode == NONE) {
    return;
  } else if (_mode == AUTO_GO) {
    if (_targetTime > 0 && millis() > _targetTime) {
      stop();
      return;
    }

    goAndTurn(_targetSpeed, 0);
  } else if (_mode == GO_STRAIGHT) {
    if (_targetTime > 0 && millis() > _targetTime) {
      stop();
      return;
    }

    _deviation += _gyro->getCourse();
    int turn = -_deviation / 1000;
    goAndTurn(_targetSpeed, turn);
  }
}

bool Truck::isRunning() {
  return _mode != NONE;
}

byte Truck::filterStickDeadZone(byte value) {
  if (value > 100 && value < 156)
    return 128;
  else
    return value;
}
