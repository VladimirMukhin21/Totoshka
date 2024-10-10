#pragma once

#include <Servo.h>
#include "Angle.h"

class Hand {
public:
  void init(byte shoulderPin, byte elbowPin, byte rotatePin, byte clawPin);
  void operate(byte stickVert, byte stickHoriz, bool altMode);
  void handToBack();
  void handToUp();
  void tinUp();
  void handToRideTheLine();
  void handToRotatePipe();
  void handToTakeTin();
  void takeTin();
  void clenchPipe();
  void unclench();
  void rotateToCenter();
  void rotate(int pos);
  void claw(int pos);
  void handToPos(int _shoulderPos = -1, int _elbowPos = -1, int _rotatePos = -1, int _clawPos = -1,
                 byte _shoulderSpeed = 6, byte _elbowSpeed = 6, byte _rotateSpeed = 5, byte _clawSpeed = 4);
  void stop();
  void tick();
  bool isRunning();

private:
  enum Mode {
    NONE,
    HAND_TO_BACK,
    TIN_UP_FROM_LOW_POS,
    TIN_UP_FROM_HIGH_POS,
    TO_RIDE_LINE,
    TO_ROTATE_PIPE,
    TO_TAKE_TIN,
    ROTATE,
    CLAW,
    HAND_TO_POS
  };

  const byte _minStick = 0;
  const byte _maxStick = 255;
  const byte _maxSpeed = 5;

  //const byte _shoulderThr = 50;
  const byte _shoulderHandToBackPos = 30;
  const byte _elbowHandToBackPos = 109;

  const byte _shoulderHandToUpPos = 30;
  const byte _elbowHandToUpPos = 0;

  const byte _rotateCenterPos = 110;

  const byte _clawOpenPos = 110;            // разжатие руки при захвате маяка
  const byte _clawOpenRotatePipePos = 105;  // разжатие руки при вращении трубок
  const byte _clawTakeTinPos = 77;          // сжатие руки при захвате маяка
  const byte _clenchPipePos = 77;           // сжатие руки при вращении трубок

  const byte _shoulderRideTheLinePos = 115;
  const byte _elbowRideTheLinePos = 0;
  const byte _rotateRideTheLinePos = 3;
  const byte _clawRideTheLinePos = 58;

  const byte _shoulderRotatePipePos = 100;
  const byte _elbowRotatePipePos = 76;

  const byte _shoulderAngleTakeTinFromHighPos = 60;
  const byte _elbowAngleTakeTinFromHighPos = 90;

  // const byte _shoulderToTakeTinPos = 154; опускание руки пока убрали, т.к. пока не надо
  // const byte _elbowToTakeTinPos = 120;

  Servo _shoulder;
  Servo _elbow;
  Servo _rotate;
  Servo _claw;

  Angle _shoulderAngle = Angle(30, 30, 154, 70);             // вверх-вниз
  Angle _elbowAngle = Angle(0, 0, 120, 70);                  // вниз-вверх
  Angle _rotateAngle = Angle(_rotateCenterPos, 3, 180, 30);  // против-по часовой (смотреть по ходу движения робота)
  Angle _clawAngle = Angle(105, 62, 130, 40);                // сжатие-разжатие

  /*#ifdef DEBUG
          unsigned long _printTime = millis();
        #endif*/

  unsigned long _tickTime = millis();
  Mode _mode = NONE;
  int _targetShoulderPos;
  int _targetElbowPos;
  int _targetRotatePos;
  int _targetClawPos;

  byte _shoulderSpeedInPoints;
  byte _elbowSpeedInPoints;
  byte _rotateSpeedInPoints;
  byte _clawSpeedInPoints;

  byte filterStickDeadZoneHoriz(byte value);
  byte filterStickDeadZoneVert(byte value);
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
    int speedVert = map(filterStickDeadZoneVert(stickVert), _minStick, _maxStick + 1, -_maxSpeed, _maxSpeed + 1);
    if (speedVert != 0) {
      _elbowAngle.addPoints(speedVert);
      _elbow.write(_elbowAngle.toDeg());
      _mode = NONE;
    }

    int speedHoriz = map(filterStickDeadZoneHoriz(stickHoriz), _minStick, _maxStick + 1, -_maxSpeed, _maxSpeed + 1);
    if (speedHoriz != 0) {
      _rotateAngle.addPoints(speedHoriz);
      _rotate.write(_rotateAngle.toDeg());
      _mode = NONE;
    }
  }
  else {
    int speedVert = map(filterStickDeadZoneVert(stickVert), _minStick, _maxStick + 1, -_maxSpeed, _maxSpeed + 1);
    if (speedVert != 0) {
      _shoulderAngle.addPoints(-speedVert);
      _shoulder.write(_shoulderAngle.toDeg());

      _elbowAngle.addPoints(-speedVert);
      _elbow.write(_elbowAngle.toDeg());

      _mode = NONE;
    }

    int speedHoriz = map(filterStickDeadZoneHoriz(stickHoriz), _minStick, _maxStick + 1, -_maxSpeed, _maxSpeed + 1);
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
  handToPos(_shoulderHandToBackPos, _elbowHandToBackPos, _rotateCenterPos);
  // HAND_TO_BACK
}

void Hand::handToUp() {
  handToPos(_shoulderHandToUpPos, _elbowHandToUpPos, _rotateCenterPos);
}

void Hand::tinUp() {
  if (_shoulderAngle.toDeg() > 100) {
    byte _shoulderAngleTakeTin = _shoulderAngle.toDeg() - 35;
    byte _elbowAngleTakeTin = _shoulderAngleTakeTin - 24;
    handToPos(_shoulderAngleTakeTin, _elbowAngleTakeTin, _rotateCenterPos);
    // TIN_UP_FROM_LOW_POS
  }
  else {
    handToPos(_shoulderAngleTakeTinFromHighPos, _elbowAngleTakeTinFromHighPos, _rotateCenterPos);
    // TIN_UP_FROM_HIGH_POS
  }
}

void Hand::handToRideTheLine() {
  handToPos(_shoulderRideTheLinePos, _elbowRideTheLinePos, _rotateRideTheLinePos, _clawRideTheLinePos, 5, 7, 5, 3);
  // TO_RIDE_LINE
}

void Hand::handToRotatePipe() {
  handToPos(_shoulderRotatePipePos, _elbowRotatePipePos, _rotateCenterPos, _clawOpenPos);
  // TO_ROTATE_PIPE
}

void Hand::handToTakeTin() {
  handToPos(-1, -1, _rotateCenterPos, _clawOpenPos);
  // handToPos(_shoulderToTakeTinPos, _elbowToTakeTinPos, _rotateCenterPos, _clawOpenPos); опускание руки пока убрали,
  // TO_TAKE_TIN                                                                           т.к. пока не надо (но оно работает)
}

void Hand::takeTin() {
  claw(_clawTakeTinPos);
}

void Hand::clenchPipe() {
  claw(_clenchPipePos);
}

void Hand::unclench() {
  claw(_clawOpenRotatePipePos);
}

void Hand::rotateToCenter() {
  rotate(_rotateCenterPos);
}

void Hand::rotate(int pos) {
  _targetRotatePos = pos;
  _mode = ROTATE;
}

void Hand::claw(int pos) {
  _targetClawPos = pos;
  _mode = CLAW;
}

void Hand::handToPos(int _shoulderPos = -1, int _elbowPos = -1, int _rotatePos = -1, int _clawPos = -1,
                     byte _shoulderSpeed = 6, byte _elbowSpeed = 6, byte _rotateSpeed = 5, byte _clawSpeed = 4) {
  _targetShoulderPos = _shoulderPos;
  _targetElbowPos = _elbowPos;
  _targetRotatePos = _rotatePos;
  _targetClawPos = _clawPos;

  _shoulderSpeedInPoints = _shoulderSpeed;
  _elbowSpeedInPoints = _elbowSpeed;
  _rotateSpeedInPoints = _rotateSpeed;
  _clawSpeedInPoints = _clawSpeed;

  _mode = HAND_TO_POS;
}

void Hand::stop() {
  _mode = NONE;
}

void Hand::tick() {
  _elbowAngle.setMinMax(0, constrain(0.75 * _shoulderAngle.toDeg() + 82.5, _elbowHandToBackPos, 120));

  if ((_mode == NONE) || (millis() - _tickTime < 1)) {
    return;
  }

  _tickTime = millis();

  int shoulderPointsPre = _shoulderAngle.toPoints();
  int elbowPointsPre = _elbowAngle.toPoints();
  int rotatePointsPre = _rotateAngle.toPoints();
  int clawPointsPre = _clawAngle.toPoints();

  if (_mode == ROTATE) {
    _rotateAngle.addPoints(constrain(_targetRotatePos - _rotateAngle.toDeg(), -5, 5));
    _rotate.write(_rotateAngle.toDeg());
  }
  else if (_mode == CLAW) {
    _clawAngle.addPoints(constrain(_targetClawPos - _clawAngle.toDeg(), -4, 4));
    _claw.write(_clawAngle.toDeg());
  }
  else if (_mode == HAND_TO_POS) {
    if (_targetShoulderPos >= 0) {
      _shoulderAngle.addPoints(constrain(_targetShoulderPos - _shoulderAngle.toDeg(), -_shoulderSpeedInPoints, _shoulderSpeedInPoints));
      _shoulder.write(_shoulderAngle.toDeg());
    }
    if (_targetElbowPos >= 0) {
      _elbowAngle.addPoints(constrain(_targetElbowPos - _elbowAngle.toDeg(), -_elbowSpeedInPoints, _elbowSpeedInPoints));
      _elbow.write(_elbowAngle.toDeg());
    }
    if (_targetRotatePos >= 0) {
      _rotateAngle.addPoints(constrain(_targetRotatePos - _rotateAngle.toDeg(), -_rotateSpeedInPoints, _rotateSpeedInPoints));
      _rotate.write(_rotateAngle.toDeg());
    }
    if (_targetClawPos >= 0) {
      _clawAngle.addPoints(constrain(_targetClawPos - _clawAngle.toDeg(), -_clawSpeedInPoints, _clawSpeedInPoints));
      _claw.write(_clawAngle.toDeg());
    }
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

byte Hand::filterStickDeadZoneHoriz(byte value) {
  if (value > 88 && value < 168)
    return 128;
  else
    return value;
}

byte Hand::filterStickDeadZoneVert(byte value) {
  if (value > 100 && value < 156)
    return 128;
  else
    return value;
}
