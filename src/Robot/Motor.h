#pragma once

class Motor {
public:
  void init(byte enPin, byte inaPin, byte inbPin, byte pwmPin);
  void go(int speed);
  void stop();
private:
  const byte _minSpeed = 0;
  const byte _maxSpeed = 255;

  byte _enPin;
  byte _inaPin;
  byte _inbPin;
  byte _pwmPin;
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

void Motor::go(int speed) {
  bool forward = speed >= 0;
  speed = abs(speed);

  if (speed < _minSpeed) {
    speed = 0;
  }
  else if (speed > _maxSpeed) {
    speed = _maxSpeed;
  }

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

void Motor::stop() {
  go(0);
}
