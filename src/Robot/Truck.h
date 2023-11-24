#pragma once

#include "Motor.h"
#include "Gyro.h"

class Truck {
public:
  void init(byte lEnPin, byte lInaPin, byte lInbPin, byte lPwmPin, byte rEnPin, byte rInaPin, byte rInbPin, byte rPwmPin, Gyro &gyro);
  void operate(byte stickVert, byte stickHoriz);
  void goAndTurn(int speed, int turn, byte smoothPercent = Motor::SMOOTH_SOFT);
  void speedGo(int speedLeft, int speedRight, byte smoothPercent = Motor::SMOOTH_SOFT);
  void autoGo(int speed, int msec = -1);
  void goStraight(int speed, bool resetDeviation = true, int msec = -1);
  void goHill(int speed, int thresholdHillAngle = 4000, int thresholdHorizAngle = 1000, int fixTimeMsec = 50, int maxTimeMsec = -1);
  void goWhilePitchInRange(int speed, int minPitchAngle, int maxPitchAngle, bool absolutePitch = true, int msec = -1);
  void stop(byte smoothPercent = Motor::SMOOTH_SOFT);
  void tick();
  bool isRunning();

private:
  enum Mode {
    NONE,
    AUTO_GO,
    GO_STRAIGHT,
    GO_TO_HILL,
    GO_TO_HORIZ,
    GO_WHILE_PITCH_IN_RANGE
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

  Mode _mode = NONE;
  int _targetSpeed = 0;
  unsigned long _targetTime = 0;
  int _thresholdHillAngle = 0;
  int _thresholdHorizAngle = 0;
  int _minPitchAngle = 0;
  int _maxPitchAngle = 0;
  bool _absolutePitch = false;
  int _fixTimeMsec = 0;                  // показывает сколько мсек надо для четкой фиксации показаний (защита от плавающих показаний)
  unsigned long _timeOfStartFixing = 0;  // время начала фиксации требуемых показаний

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
    goAndTurn(speed, turn, Motor::SMOOTH_OFF);
  }
  else {
    if (speed != 0 || turn != 0) {
      goAndTurn(speed, turn, Motor::SMOOTH_OFF);
      _mode = NONE;
    }
  }
}

void Truck::goAndTurn(int speed, int turn, byte smoothPercent = Motor::SMOOTH_SOFT) {
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

  speedGo(speedLeft, speedRight, smoothPercent);
}

void Truck::speedGo(int speedLeft, int speedRight, byte smoothPercent = Motor::SMOOTH_SOFT) {
  _left.go(speedLeft, smoothPercent);
  _right.go(speedRight, smoothPercent);
}

void Truck::autoGo(int speed, int msec = -1) {
  _targetSpeed = speed;
  _targetTime = -1;
  if (msec > 0) {
    _targetTime = millis() + msec;
  }

  _mode = AUTO_GO;
}

void Truck::goStraight(int speed, bool resetDeviation = true, int msec = -1) {
  _targetSpeed = speed;
  _targetTime = -1;
  if (msec > 0) {
    _targetTime = millis() + msec;
  }

  if (resetDeviation) {
    _deviation = 0;
  }

  _mode = GO_STRAIGHT;
}

void Truck::goHill(int speed, int thresholdHillAngle = 4000, int thresholdHorizAngle = 1000, int fixTimeMsec = 50, int maxTimeMsec = -1) {
  _targetSpeed = speed;
  _thresholdHillAngle = thresholdHillAngle;
  _thresholdHorizAngle = thresholdHorizAngle;
  _fixTimeMsec = fixTimeMsec;
  _targetTime = -1;
  if (maxTimeMsec > 0) {
    _targetTime = millis() + maxTimeMsec;
  }

  _timeOfStartFixing = 0;
  _deviation = 0;
  _mode = GO_TO_HILL;
}

void Truck::goWhilePitchInRange(int speed, int minPitchAngle, int maxPitchAngle, bool absolutePitch = true, int msec = -1) {
  _targetSpeed = speed;
  _minPitchAngle = minPitchAngle;
  _maxPitchAngle = maxPitchAngle;
  _absolutePitch = absolutePitch;
  _targetTime = -1;
  if (msec > 0) {
    _targetTime = millis() + msec;
  }

  _deviation = 0;
  _mode = GO_WHILE_PITCH_IN_RANGE;
}

void Truck::stop(byte smoothPercent = Motor::SMOOTH_SOFT) {
  _left.stop(smoothPercent);
  _right.stop(smoothPercent);
  _mode = NONE;
}

void Truck::tick() {
  _left.tick();
  _right.tick();

  if (_mode == NONE) {
    return;
  }
  else if (_mode == AUTO_GO) {
    if (_targetTime > 0 && millis() > _targetTime) {
      stop(Motor::SMOOTH_OFF);
      return;
    }

    goAndTurn(_targetSpeed, 0, Motor::SMOOTH_OFF);
  }
  else if (_mode == GO_STRAIGHT) {
    if (_targetTime > 0 && millis() > _targetTime) {
      stop();
      return;
    }

    _deviation += _gyro->getDeltaCourse();
    int turn = -_deviation / 1000;
    goAndTurn(_targetSpeed, turn);
  }
  else if (_mode == GO_TO_HILL) {
    if (_targetTime > 0 && millis() > _targetTime) {
      stop();
      return;
    }

    // Serial.print(0); Serial.print("\t"); Serial.println(_gyro->getPitch());
    if (abs(_gyro->getPitch()) >= _thresholdHillAngle) {
      if (_timeOfStartFixing <= 0) {
        _timeOfStartFixing = millis();
      }

      if (millis() - _timeOfStartFixing > _fixTimeMsec) {
        _timeOfStartFixing = 0;
        _mode = GO_TO_HORIZ;
        return;
      }
    }
    else {
      _timeOfStartFixing = 0;
    }

    _deviation += _gyro->getDeltaCourse();
    int turn = -_deviation / 1000;
    goAndTurn(_targetSpeed, turn);
  }
  else if (_mode == GO_TO_HORIZ) {
    if (_targetTime > 0 && millis() > _targetTime) {
      stop();
      return;
    }

    int pitch = _gyro->getPitch();
    if (abs(pitch) <= _thresholdHorizAngle) {
      if (_timeOfStartFixing <= 0) {
        _timeOfStartFixing = millis();
      }

      if (millis() - _timeOfStartFixing > _fixTimeMsec) {
        _timeOfStartFixing = 0;
        stop();
        return;
      }
    }
    else {
      _timeOfStartFixing = 0;
    }

    _deviation += _gyro->getDeltaCourse();
    int turn = -_deviation / 1000;
    goAndTurn(_targetSpeed, turn);
  }
  else if (_mode == GO_WHILE_PITCH_IN_RANGE) {
    if (_targetTime > 0 && millis() > _targetTime) {
      stop();
      return;
    }

    int pitch = _gyro->getPitch();
    if (_absolutePitch) {
      pitch = abs(pitch);
    }

    if (pitch < _minPitchAngle || pitch > _maxPitchAngle) {
      stop();
      return;
    }

    _deviation += _gyro->getDeltaCourse();
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
