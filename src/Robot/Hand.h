#pragma once

#include <Servo.h>
#include "Angle.h"

class Hand {
public:
  void init(byte shoulderPin, byte elbowPin, byte rotatePin, byte clawPin);
  void operate(byte stickVert, byte stickHoriz, bool altMode);
  void upHand();
  void handRideTheLine();
  void stop();
  void tick();

private:
  const byte _minStick = 0;
  const byte _maxStick = 255;
  const byte _maxSpeed = 5;

  const byte _shoulderThr = 50;
  const byte _elbowUpHandPos = 108;
  const byte _rotateCenterPos = 110;

  const byte _shoulderRideTheLinePos = 115;
  const byte _elbowRideTheLinePos = 0;
  const byte _rotateRideTheLinePos = 3;
  const byte _clawRideTheLinePos = 58;


  Servo _shoulder;
  Servo _elbow;
  Servo _rotate;
  Servo _claw;

  Angle _shoulderAngle = Angle(30, 30, 154, 100);  // (115, 30, 154, 100)
  Angle _elbowAngle = Angle(0, 0, 120, 100);
  Angle _rotateAngle = Angle(_rotateCenterPos, 0, 180, 30);  // (3, 0, 180, 30)
  Angle _clawAngle = Angle(97, 58, 165, 50);                 // (58, 58, 180, 50)

  /*#ifdef DEBUG
        unsigned long _printTime = millis();
      #endif*/

  unsigned long _tickTime = millis();
  bool _upHandMode = false;
  bool _handRideTheLineMode = false;

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
  /*if(_shoulderAngle.toDeg() > shoulderThr){
    constrain(_elbowAngle.toDeg() ,0 ,_elbowUpHandPos);
    }*/

  if (altMode) {
    int speedVert = map(filterStickDeadZone(stickVert), _minStick, _maxStick + 1, -_maxSpeed, _maxSpeed + 1);
    if (speedVert != 0) {
      _elbowAngle.addPoints(speedVert);
      _elbow.write(_elbowAngle.toDeg());
      _upHandMode = false;
      _handRideTheLineMode = false;
    }

    int speedHoriz = map(filterStickDeadZone(stickHoriz), _minStick, _maxStick + 1, -_maxSpeed, _maxSpeed + 1);
    if (speedHoriz != 0) {
      _rotateAngle.addPoints(speedHoriz);
      _rotate.write(_rotateAngle.toDeg());
      _upHandMode = false;
      _handRideTheLineMode = false;
    }
  } else {
    int speedVert = map(filterStickDeadZone(stickVert), _minStick, _maxStick + 1, -_maxSpeed, _maxSpeed + 1);
    if (speedVert != 0) {
      _shoulderAngle.addPoints(-speedVert);
      _shoulder.write(_shoulderAngle.toDeg());

      _elbowAngle.addPoints(-speedVert);
      _elbow.write(_elbowAngle.toDeg());

      _upHandMode = false;
      _handRideTheLineMode = false;
    }

    int speedHoriz = map(filterStickDeadZone(stickHoriz), _minStick, _maxStick + 1, -_maxSpeed, _maxSpeed + 1);
    if (speedHoriz != 0) {
      _clawAngle.addPoints(-speedHoriz);
      _claw.write(_clawAngle.toDeg());
      _upHandMode = false;
      _handRideTheLineMode = false;
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

void Hand::handRideTheLine() {
  _handRideTheLineMode = true;
}

void Hand::stop() {
  _upHandMode = false;
  _handRideTheLineMode = false;
}

void Hand::tick() {
  _elbowAngle.setMinMax(0, constrain(0.75 * _shoulderAngle.toDeg() + 82.5, _elbowUpHandPos, 120));


  if ((_handRideTheLineMode) && (millis() - _tickTime >= 1)) {
    _tickTime = millis();

    int shoulderPointsPre = _shoulderAngle.toPoints();
    int elbowPointsPre = _elbowAngle.toPoints();
    int rotatePointsPre = _rotateAngle.toPoints();
    int clawPointsPre = _clawAngle.toPoints();

    _shoulderAngle.addPoints(constrain(_shoulderRideTheLinePos - _shoulderAngle.toDeg(), -5, 5));
    _shoulder.write(_shoulderAngle.toDeg());

    _elbowAngle.addPoints(-8);
    _elbow.write(_elbowAngle.toDeg());

    _rotateAngle.addPoints(constrain(_rotateRideTheLinePos - _rotateAngle.toDeg(), -5, 5));
    _rotate.write(_rotateAngle.toDeg());

    _clawAngle.addPoints(-3);
    _claw.write(_clawAngle.toDeg());

    if (shoulderPointsPre == _shoulderAngle.toPoints()
        && elbowPointsPre == _elbowAngle.toPoints()
        && rotatePointsPre == _rotateAngle.toPoints()
        && clawPointsPre == _clawAngle.toPoints()) {
      _handRideTheLineMode = false;
    }
  }


  if ((_upHandMode) && (millis() - _tickTime >= 1)) {
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
