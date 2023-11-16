#pragma once

#include <Servo.h>
#include "Angle.h"

class Hand {
  public:
    void init(byte shoulderPin, byte elbowPin, byte rotatePin, byte clawPin);
    void operate(byte stickVert, byte stickHoriz, bool altMode);
    void handToBack();
    void handUp();
    void handToRideTheLine();
    void handToTakeTin();
    void takeTin();
    void stop();
    void tick();
    bool isRunning();

  private:
    enum Mode {
      NONE,
      HAND_TO_BACK,
      HAND_UP,
      TO_RIDE_LINE,
      TO_TAKE_TIN,
      TAKE_TIN
    };

    const byte _minStick = 0;
    const byte _maxStick = 255;
    const byte _maxSpeed = 5;

    const byte _shoulderThr = 50;
    const byte _elbowUpHandPos = 108;
    const byte _rotateCenterPos = 110;
    const byte _clawOpenPos = 97;
    const byte _clawTakeTinPos = 73;

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
    Angle _clawAngle = Angle(97, 61, 110, 50);                 // (58, 58, 180, 50) 97 58 165 50

    /*#ifdef DEBUG
          unsigned long _printTime = millis();
        #endif*/

    unsigned long _tickTime = millis();
    Mode _mode = NONE;

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
      _mode = NONE;
    }

    int speedHoriz = map(filterStickDeadZone(stickHoriz), _minStick, _maxStick + 1, -_maxSpeed, _maxSpeed + 1);
    if (speedHoriz != 0) {
      _rotateAngle.addPoints(speedHoriz);
      _rotate.write(_rotateAngle.toDeg());
      _mode = NONE;
    }
  } else {
    int speedVert = map(filterStickDeadZone(stickVert), _minStick, _maxStick + 1, -_maxSpeed, _maxSpeed + 1);
    if (speedVert != 0) {
      _shoulderAngle.addPoints(-speedVert);
      _shoulder.write(_shoulderAngle.toDeg());

      _elbowAngle.addPoints(-speedVert);
      _elbow.write(_elbowAngle.toDeg());

      _mode = NONE;
    }

    int speedHoriz = map(filterStickDeadZone(stickHoriz), _minStick, _maxStick + 1, -_maxSpeed, _maxSpeed + 1);
    if (speedHoriz != 0) {
      _clawAngle.addPoints(-speedHoriz);
      _claw.write(_clawAngle.toDeg());
      _mode = NONE;
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

void Hand::handToBack() {
  _mode = HAND_TO_BACK;
}

void Hand::handUp() {
  _mode = HAND_UP;
}

void Hand::handToRideTheLine() {
  _mode = TO_RIDE_LINE;
}

void Hand::handToTakeTin() {
  _mode = TO_TAKE_TIN;
}

void Hand::takeTin() {
  _mode = TAKE_TIN;
}

void Hand::stop() {
  _mode = NONE;
}

void Hand::tick() {
  _elbowAngle.setMinMax(0, constrain(0.75 * _shoulderAngle.toDeg() + 82.5, _elbowUpHandPos, 120));

  if ((_mode == NONE) || (millis() - _tickTime < 1)) {
    return;
  }

  _tickTime = millis();

  int shoulderPointsPre = _shoulderAngle.toPoints();
  int elbowPointsPre = _elbowAngle.toPoints();
  int rotatePointsPre = _rotateAngle.toPoints();
  int clawPointsPre = _clawAngle.toPoints();

  switch (_mode) {
    case HAND_TO_BACK:
      _shoulderAngle.addPoints(-5);
      _shoulder.write(_shoulderAngle.toDeg());

      _elbowAngle.addPoints(constrain(_elbowUpHandPos - _elbowAngle.toDeg(), -5, 5));
      _elbow.write(_elbowAngle.toDeg());

      _rotateAngle.addPoints(constrain(_rotateCenterPos - _rotateAngle.toDeg(), -5, 5));
      _rotate.write(_rotateAngle.toDeg());

      break;

    case HAND_UP:
      _shoulderAngle.addPoints(-5);
      _shoulder.write(_shoulderAngle.toDeg());

      _elbowAngle.addPoints(-5);
      _elbow.write(_elbowAngle.toDeg());

      _rotateAngle.addPoints(constrain(_rotateCenterPos - _rotateAngle.toDeg(), -5, 5));
      _rotate.write(_rotateAngle.toDeg());

      break;

    case TO_RIDE_LINE:
      _shoulderAngle.addPoints(constrain(_shoulderRideTheLinePos - _shoulderAngle.toDeg(), -5, 5));
      _shoulder.write(_shoulderAngle.toDeg());

      _elbowAngle.addPoints(-8);
      _elbow.write(_elbowAngle.toDeg());

      _rotateAngle.addPoints(constrain(_rotateRideTheLinePos - _rotateAngle.toDeg(), -5, 5));
      _rotate.write(_rotateAngle.toDeg());

      _clawAngle.addPoints(-3);
      _claw.write(_clawAngle.toDeg());

      break;

    case TO_TAKE_TIN:
      _shoulderAngle.addPoints(5);
      _shoulder.write(_shoulderAngle.toDeg());

      _elbowAngle.addPoints(5);
      _elbow.write(_elbowAngle.toDeg());

      _rotateAngle.addPoints(constrain(_rotateCenterPos - _rotateAngle.toDeg(), -5, 5));
      _rotate.write(_rotateAngle.toDeg());

      _clawAngle.addPoints(constrain(_clawOpenPos - _clawAngle.toDeg(), -3, 3));
      _claw.write(_clawAngle.toDeg());

      break;

    case TAKE_TIN:
      _clawAngle.addPoints(constrain(_clawTakeTinPos - _clawAngle.toDeg(), -3, 3));
      _claw.write(_clawAngle.toDeg());

      break;
  }

  if (shoulderPointsPre == _shoulderAngle.toPoints()
      && elbowPointsPre == _elbowAngle.toPoints()
      && rotatePointsPre == _rotateAngle.toPoints()
      && clawPointsPre == _clawAngle.toPoints()) {
    _mode = NONE;
  }
}

bool Hand::isRunning() {
  return _mode != NONE;
}

byte Hand::filterStickDeadZone(byte value) {
  if (value > 88 && value < 168)
    return 128;
  else
    return value;
}
