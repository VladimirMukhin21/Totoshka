#pragma once

#include <Servo.h>
#include "Angle.h"

class Hand {
  public:
    void init(byte shoulderPin, byte elbowPin, byte rotatePin, byte clawPin);
    void operate(byte stickVert, byte stickHoriz, bool altMode);
    void upHand();
    void stop();
    void tick();

  private:
    const byte _minStick = 0;
    const byte _maxStick = 255;
    const byte _maxSpeed = 5;
    const byte _rotateCenterPos = 90;
    const byte _elbowUpHandPos = 105;
    //const byte _shoulderThr = 50;

    Servo _shoulder;
    Servo _elbow;
    Servo _rotate;
    Servo _claw;

    Angle _shoulderAngle = Angle(154, 31, 154, 100); // 38
    Angle _elbowAngle = Angle(120, 0, 120, 100);
    Angle _rotateAngle = Angle(_rotateCenterPos, 0, 180, 30);
    Angle _clawAngle = Angle(90, 58, 180, 50); // 56

    /*#ifdef DEBUG
        unsigned long _printTime = millis();
      #endif*/

    unsigned long _tickTime = millis();
    bool _upHandMode = false;

    byte filterStickDeadZone(byte value);
};

void Hand::init(byte shoulderPin, byte elbowPin, byte rotatePin, byte clawPin) {
  _shoulder.attach(shoulderPin);
  _shoulder.write(_shoulderAngle.toDeg());

  _elbow.attach(elbowPin);
  _elbow.write(_elbowAngle.toDeg());

  _rotate.attach(rotatePin);
  _rotate.write(_rotateAngle.toDeg());

  _claw.attach(clawPin);
  _claw.write(_clawAngle.toDeg());
}

void Hand::operate(byte stickVert, byte stickHoriz, bool altMode) {
  /*if(_shoulderAngle.toPoints() > shoulderThr * 100){
    constrain(_elbowAngle.toDeg() ,0 ,_elbowUpHandPos)
  }*/

  if (altMode) {
      int speedVert = map(filterStickDeadZone(stickVert), _minStick, _maxStick + 1, -_maxSpeed, _maxSpeed + 1);
      if (speedVert != 0) {
        _elbowAngle.addPoints(speedVert);
        _elbow.write(_elbowAngle.toDeg());
        _upHandMode = false;
      }

      int speedHoriz = map(filterStickDeadZone(stickHoriz), _minStick, _maxStick + 1, -_maxSpeed, _maxSpeed + 1);
      if (speedHoriz != 0) {
        _rotateAngle.addPoints(speedHoriz);
        _rotate.write(_rotateAngle.toDeg());
        _upHandMode = false;
      }
    }
    else {
      int speedVert = map(filterStickDeadZone(stickVert), _minStick, _maxStick + 1, -_maxSpeed, _maxSpeed + 1);
      if (speedVert != 0) {
        _shoulderAngle.addPoints(-speedVert);
        _shoulder.write(_shoulderAngle.toDeg());

        _elbowAngle.addPoints(-speedVert);
        _elbow.write(_elbowAngle.toDeg());

        _upHandMode = false;
      }

      int speedHoriz = map(filterStickDeadZone(stickHoriz), _minStick, _maxStick + 1, -_maxSpeed, _maxSpeed + 1);
      if (speedHoriz != 0) {
        _clawAngle.addPoints(-speedHoriz);
        _claw.write(_clawAngle.toDeg());
        _upHandMode = false;
      }

      /*#ifdef DEBUG
          if (millis() - _printTime >= 1000) {
            _printTime = millis();
            Serial.print("stick = ");
            Serial.print(stickHoriz);
            Serial.print("\tangle = ");
            Serial.print(_elbowAngle.toDeg());
            Serial.print("\tspeed = ");
            Serial.print(speedHoriz);
            Serial.println();
          }
        #endif*/
    }
}

void Hand::upHand() {
  _upHandMode = true;
}

void Hand::stop() {
  _upHandMode = false;
}

void Hand::tick() {
  if (!_upHandMode) {
    return;
  }

  if (millis() - _tickTime >= 1) {
    _tickTime = millis();

    int shoulderPointsPre = _shoulderAngle.toPoints();
    int elbowPointsPre = _elbowAngle.toPoints();
    int rotatePointsPre = _rotateAngle.toPoints();

    _shoulderAngle.addPoints(-5);
    _shoulder.write(_shoulderAngle.toDeg());

    _elbowAngle.addPoints(constrain(_elbowUpHandPos - _elbowAngle.toDeg(), -5, 5));
    _elbow.write(_elbowAngle.toDeg());

    _rotateAngle.addPoints(constrain(_rotateCenterPos - _rotateAngle.toDeg(), -5, 5));
    _rotate.write(_rotateAngle.toDeg());

    if (shoulderPointsPre == _shoulderAngle.toPoints()
        && elbowPointsPre == _elbowAngle.toPoints()
        && rotatePointsPre == _rotateAngle.toPoints()) {
      _upHandMode = false;
    }
  }
}

byte Hand::filterStickDeadZone(byte value) {
  if (value > 88 && value < 168)
    return 128;
  else
    return value;
}
