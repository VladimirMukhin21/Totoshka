#pragma once

class Motor {
public:
  static const byte SMOOTH_OFF = 0;
  static const byte SMOOTH_SOFT = 10;
  static const byte SMOOTH_HARD = 50;

  void init(byte enPin, byte inaPin, byte inbPin, byte pwmPin);
  void go(int speed, byte smoothPercent = SMOOTH_SOFT);
  void stop(byte smoothPercent = SMOOTH_SOFT);
  void tick();
private:
  static const byte MIN_SPEED = 0;
  static const byte MAX_SPEED = 255;
  static const byte SMOOTH_PERIOD = 100;

  byte _enPin;
  byte _inaPin;
  byte _inbPin;
  byte _pwmPin;

  int _targetSpeed = 0;
  int _currentSpeed = 0;
  byte _smoothPercent = SMOOTH_OFF;

  unsigned long _tickTime = millis();

  int smooth(int speed);
  void setSpeed(int speed);
  int constrainSpeed(int speed);
};

void Motor::init(byte enPin, byte inaPin, byte inbPin, byte pwmPin) {
  _enPin = enPin;
  _inaPin = inaPin;
  _inbPin = inbPin;
  _pwmPin = pwmPin;

  pinMode(enPin, OUTPUT);
  pinMode(inaPin, OUTPUT);
  pinMode(inbPin, OUTPUT);
  pinMode(pwmPin, OUTPUT);

  digitalWrite(enPin, HIGH);
}

void Motor::go(int speed, byte smoothPercent = SMOOTH_SOFT) {
  _targetSpeed = constrainSpeed(speed);
  _smoothPercent = smoothPercent;
  int smoothed = smooth(_targetSpeed);
  setSpeed(smoothed);
}

void Motor::stop(byte smoothPercent = SMOOTH_SOFT) {
  go(0, smoothPercent);
}

void Motor::tick() {
  if (_smoothPercent <= 0
      || _currentSpeed == _targetSpeed
      || millis() - _tickTime < SMOOTH_PERIOD) {
    return;
  }

  _tickTime = millis();

  int smoothed = smooth(_targetSpeed);
  setSpeed(smoothed);
}

int Motor::smooth(int speed) {
  if (_smoothPercent <= 0) {
    return speed;
  }

  int min = _currentSpeed * (100 - _smoothPercent) / 100;
  if (speed <= min) {
    return min;
  }

  int max = _currentSpeed * (100 + _smoothPercent) / 100;
  if (speed >= max) {
    return max;
  }

  return speed;
}

void Motor::setSpeed(int speed) {
  speed = constrainSpeed(speed);
  _currentSpeed = speed;

  bool forward = speed >= 0;
  speed = abs(speed);

  digitalWrite(_inaPin, forward);
  digitalWrite(_inbPin, !forward);
  analogWrite(_pwmPin, speed);

  /*if (speed > 0) {
    digitalWrite(_inaPin, forward);
    digitalWrite(_inbPin, !forward);
    analogWrite (_pwmPin, speed);
    digitalWrite(_enPin, HIGH);
    }
    else {
    analogWrite (_pwmPin, 0);
    digitalWrite(_enPin, LOW);
    }*/
}

int Motor::constrainSpeed(int speed) {
  if (abs(speed) < MIN_SPEED) {
    return 0;
  }
  else if (speed > MAX_SPEED) {
    return MAX_SPEED;
  }
  else if (speed < -MAX_SPEED) {
    return -MAX_SPEED;
  }

  return speed;
}
