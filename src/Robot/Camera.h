#pragma once

#include <Servo.h>
#include "Angle.h"

class Camera {
  public:
    void init(byte servoPin, int startDeg, int minDeg, int maxDeg, int countPointsInDeg = 100);
    void operate(byte stickVert);
    void moveTo(int deg);
    void toInitialPos();
    void stop();
    void tick();
    bool isRunning();

  private:
    const byte _minStick = 255;
    const byte _maxStick = 0;
    const byte _maxSpeed = 5;

    Servo _servo;
    Angle _direction;

    //#define DEBUG
#ifdef DEBUG
    unsigned long _printTime = millis();
#endif

    unsigned long _tickTime = millis();
    int _targetDeg;
    bool _isRunning = false;

    byte filterStickDeadZone(byte value);
};

void Camera::init(byte servoPin, int startDeg, int minDeg, int maxDeg, int countPointsInDeg = 100) {
  _direction.init(startDeg, minDeg, maxDeg, countPointsInDeg);
  _servo.attach(servoPin);
  _servo.write(_direction.toDeg());
}

void Camera::operate(byte stickVert) {
  int speedVert = map(filterStickDeadZone(stickVert), _minStick, _maxStick + 1, -_maxSpeed, _maxSpeed + 1);
  if (speedVert != 0) {
    _isRunning = false;

    _direction.addPoints(speedVert);
    _servo.write(_direction.toDeg());
  }

/*#ifdef DEBUG
  if (millis() - _printTime >= 1000) {
    _printTime = millis();
    Serial.print("stick = ");
    Serial.print(stickVert);
    Serial.print("\tangle = ");
    Serial.print(_direction.toDeg());
    Serial.print("\tspeed = ");
    Serial.print(speedVert);
    Serial.println();
  }
#endif*/
}

void Camera::moveTo(int deg) {
  _targetDeg = deg;
  _isRunning = true;
}

void Camera::toInitialPos() {
  moveTo(10);
}

void Camera::stop() {
  _isRunning = false;
}

void Camera::tick() {
  if (!_isRunning) {
    return;
  }

  if (millis() - _tickTime >= 1) {
    _tickTime = millis();

    int delta = _direction.toDeg() - _targetDeg;
    if (delta > 1) {
      _direction.addPoints(-5);
      _servo.write(_direction.toDeg());
    }
    else if (delta < -1) {
      _direction.addPoints(5);
      _servo.write(_direction.toDeg());
    }
    else {
      _isRunning = false;
    }
  }
}

bool Camera::isRunning() {
  return _isRunning;
}

byte Camera::filterStickDeadZone(byte value) {
  if (value > 88 && value < 168)
    return 128;
  else
    return value;
}
