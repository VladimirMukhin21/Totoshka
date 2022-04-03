#pragma once

#include <Servo.h>
#include "Angle.h"

class Camera {
  public:
    void init(byte cameraPin);
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

    Servo _camera;

    Angle _cameraAngle = Angle(0, 0, 180, 25);

    //#define DEBUG
#ifdef DEBUG
    unsigned long _printTime = millis();
#endif

    unsigned long _tickTime = millis();
    int _targetDeg;
    bool _isRunning = false;

    byte filterStickDeadZone(byte value);
};

void Camera::init(byte cameraPin) {
  _camera.attach(cameraPin);
  _camera.write(_cameraAngle.toDeg());
}

void Camera::operate(byte stickVert) {
  int speedVert = map(filterStickDeadZone(stickVert), _minStick, _maxStick + 1, -_maxSpeed, _maxSpeed + 1);
  if (speedVert != 0) {
    _isRunning = false;

    _cameraAngle.addPoints(speedVert);
    _camera.write(_cameraAngle.toDeg());
  }

#ifdef DEBUG
  if (millis() - _printTime >= 1000) {
    _printTime = millis();
    Serial.print("stick = ");
    Serial.print(stickVert);
    Serial.print("\tangle = ");
    Serial.print(_cameraAngle.toDeg());
    Serial.print("\tspeed = ");
    Serial.print(speedVert);
    Serial.println();
  }
#endif
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

    int delta = _cameraAngle.toDeg() - _targetDeg;
    if (delta > 1) {
      _cameraAngle.addPoints(-5);
      _camera.write(_cameraAngle.toDeg());
    }
    else if (delta < -1) {
      _cameraAngle.addPoints(5);
      _camera.write(_cameraAngle.toDeg());
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
