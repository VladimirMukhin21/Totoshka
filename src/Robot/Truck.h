#pragma once

#include "Motor.h"
#include "Gyro.h"

class Truck {
public:
  void init(byte lEnPin, byte lInaPin, byte lInbPin, byte lPwmPin, byte rEnPin, byte rInaPin, byte rInbPin, byte rPwmPin, Gyro &gyro);
  void operate(byte stickVert, byte stickHoriz);
  void goAndTurn(int speed, int turn, byte smoothStep = Motor::SMOOTH_SOFT);
  void speedGo(int speedLeft, int speedRight, byte smoothStep = Motor::SMOOTH_SOFT);
  void autoGo(int speed, int msec = -1);
  void goStraight(int speed, bool resetDeviation = true, int msec = -1);
  void goHill(int speed, int thresholdHillPitch = 4000, int thresholdHorizPitch = 1000, int fixTimeMsec = 50, int maxTimeMsec = -1);
  void goWhilePitchInRange(int speed, int minPitch, int maxPitch, bool absolutePitch = true, int msec = -1);
  void turn(long turnAngle, int msec = -1);
  void stop(byte smoothStep = Motor::SMOOTH_HARD);
  void tick();
  bool isRunning();

private:
  enum Mode {
    NONE,
    AUTO_GO,
    GO_STRAIGHT,
    GO_TO_HILL,
    GO_TO_HORIZ,
    GO_WHILE_PITCH_IN_RANGE,
    TURN
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
  int _thresholdHillPitch = 0;
  int _thresholdHorizPitch = 0;
  int _minPitch = 0;
  int _maxPitch = 0;
  bool _absolutePitch = false;
  int _fixTimeMsec = 0;                  // показывает сколько мсек надо для четкой фиксации показаний (защита от плавающих показаний)
  unsigned long _timeOfStartFixing = 0;  // время начала фиксации требуемых показаний

  byte filterStickDeadZone(byte value);
};

void Truck::init(byte lEnPin, byte lInaPin, byte lInbPin, byte lPwmPin, byte rEnPin, byte rInaPin, byte rInbPin, byte rPwmPin, Gyro &gyro) {
  _left.init(lEnPin, lInaPin, lInbPin, lPwmPin);
  _right.init(rEnPin, rInaPin, rInbPin, rPwmPin, 90, 60);
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

void Truck::goAndTurn(int speed, int turn, byte smoothStep = Motor::SMOOTH_SOFT) {
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

  speedGo(speedLeft, speedRight, smoothStep);
}

void Truck::speedGo(int speedLeft, int speedRight, byte smoothStep = Motor::SMOOTH_SOFT) {
  _left.go(speedLeft, smoothStep);
  _right.go(speedRight, smoothStep);
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

void Truck::goHill(int speed, int thresholdHillPitch = 4000, int thresholdHorizPitch = 1000, int fixTimeMsec = 50, int maxTimeMsec = -1) {
  _targetSpeed = speed;
  _thresholdHillPitch = thresholdHillPitch;
  _thresholdHorizPitch = thresholdHorizPitch;
  _fixTimeMsec = fixTimeMsec;
  _targetTime = -1;
  if (maxTimeMsec > 0) {
    _targetTime = millis() + maxTimeMsec;
  }

  _timeOfStartFixing = 0;
  _deviation = 0;
  _mode = GO_TO_HILL;
}

void Truck::goWhilePitchInRange(int speed, int minPitch, int maxPitch, bool absolutePitch = true, int msec = -1) {
  _targetSpeed = speed;
  _minPitch = minPitch;
  _maxPitch = maxPitch;
  _absolutePitch = absolutePitch;
  _targetTime = -1;
  if (msec > 0) {
    _targetTime = millis() + msec;
  }

  _deviation = 0;
  _mode = GO_WHILE_PITCH_IN_RANGE;
}

void Truck::turn(long turnAngle, int msec = -1) {
  _targetTime = -1;
  if (msec > 0) {
    _targetTime = millis() + msec;
  }

  _deviation = -turnAngle;
  _mode = TURN;
}

void Truck::stop(byte smoothStep = Motor::SMOOTH_HARD) {
  _left.stop(smoothStep);
  _right.stop(smoothStep);
  _mode = NONE;
  if (smoothStep == Motor::SMOOTH_OFF) {
    // сглаживание отключено => стоп сразу => гироскоп можно выключить
    // полезно, например, при захвате банки, чтобы рука не тормозила
    _gyro->disable();
  }
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
    int turn = -_deviation / 1800;
    goAndTurn(_targetSpeed, turn, Motor::SMOOTH_OFF);
  }
  else if (_mode == GO_TO_HILL) {
    if (_targetTime > 0 && millis() > _targetTime) {
      stop();
      return;
    }

    // Serial.print(0); Serial.print("\t"); Serial.println(_gyro->getPitch());
    if (abs(_gyro->getPitch()) >= _thresholdHillPitch) {
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

    goAndTurn(_targetSpeed, 0);
  }
  else if (_mode == GO_TO_HORIZ) {
    if (_targetTime > 0 && millis() > _targetTime) {
      stop();
      return;
    }

    int pitch = _gyro->getPitch();
    if (abs(pitch) <= _thresholdHorizPitch) {
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

    goAndTurn(_targetSpeed, 0);
  }
  else if (_mode == GO_WHILE_PITCH_IN_RANGE) {
    if (_targetTime > 0 && millis() > _targetTime) {
      stop(Motor::SMOOTH_OFF);
      return;
    }

    int pitch = _gyro->getPitch();
    if (_absolutePitch) {
      pitch = abs(pitch);
    }

    if (pitch < _minPitch || pitch > _maxPitch) {
      stop(Motor::SMOOTH_OFF);
      return;
    }

    goAndTurn(_targetSpeed, 0, Motor::SMOOTH_SOFT);
  }
  else if (_mode == TURN) {
    if (_targetTime > 0 && millis() > _targetTime) {
      stop(Motor::SMOOTH_OFF);
      return;
    }

    // int course = _gyro->getDeltaCourse();
    // long residual = _deviation - course;
    _deviation += _gyro->getDeltaCourse();
    // int turn = -_deviation / 1800;
    // int turn = -residual / 1800;
    int turn = 0;
    // if (residual > 0) {
    //   turn = _maxTurn;
    // }
    // else if (residual < 0) {
    //   turn = -_maxTurn;
    // }

    if (_deviation > 0) {
      turn = -_maxTurn / 2;
    }
    else if (_deviation < 0) {
      turn = _maxTurn / 2;
    }

    // Serial.print(course);
    // Serial.print("\t");
    // Serial.print(residual);
    // Serial.print("\t");
    // Serial.print(_deviation);
    // Serial.print("\t");
    // Serial.print(turn);
    // Serial.print("\t");
    // Serial.print("0\t500\t-500");  // линии для масштаба на графике
    // Serial.println();

    goAndTurn(0, turn, Motor::SMOOTH_HARD);

    // if (abs(residual) < 5000) {
    if (abs(_deviation) < 2000) {
      stop(Motor::SMOOTH_OFF);
      return;
    }
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
