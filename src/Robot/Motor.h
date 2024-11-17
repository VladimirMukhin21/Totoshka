#pragma once

class Motor {
public:
  static const byte SMOOTH_OFF = 0;
  static const byte SMOOTH_SOFT = 10;
  static const byte SMOOTH_HARD = 50;

  void init(byte enPin, byte inaPin, byte inbPin, byte pwmPin, byte _truncForwardSpeed = 0, byte _truncBackSpeed = 0);
  void go(int speed, byte smoothStep = SMOOTH_SOFT);
  void stop(byte smoothStep = SMOOTH_HARD);
  void tick();
private:
  static const byte MIN_SPEED = 0;
  static const byte MAX_SPEED = 255;
  static const byte SMOOTH_PERIOD = 50;

  byte _enPin;
  byte _inaPin;
  byte _inbPin;
  byte _pwmPin;
  byte _truncatedForwardMaxSpeed = MAX_SPEED;
  byte _truncatedBackMaxSpeed = MAX_SPEED;

  int _targetSpeed = 0;
  int _currentSpeed = 0;
  byte _smoothStep = SMOOTH_OFF;

  unsigned long _tickTime = millis();
  unsigned long _smoothTime = millis();

  int smooth(int speed);
  void setSpeed(int speed);
  int constrainSpeed(int speed);
};

void Motor::init(byte enPin, byte inaPin, byte inbPin, byte pwmPin, byte _truncForwardSpeed = 0, byte _truncBackSpeed = 0) {
  _enPin = enPin;
  _inaPin = inaPin;
  _inbPin = inbPin;
  _pwmPin = pwmPin;

  pinMode(enPin, OUTPUT);
  pinMode(inaPin, OUTPUT);
  pinMode(inbPin, OUTPUT);
  pinMode(pwmPin, OUTPUT);

  digitalWrite(enPin, HIGH);

  _truncatedForwardMaxSpeed = MAX_SPEED - _truncForwardSpeed;
  _truncatedBackMaxSpeed = MAX_SPEED - _truncBackSpeed;
}

void Motor::go(int speed, byte smoothStep = SMOOTH_SOFT) {
  _targetSpeed = constrainSpeed(speed);
  _smoothStep = smoothStep;
  int smoothed = smooth(_targetSpeed);
  setSpeed(smoothed);

  // Serial.print("_targetSpeed\t");
  // Serial.println(_targetSpeed);
  // Serial.print("_smoothStep\t");
  // Serial.println(_smoothStep);
  // Serial.print("smoothed\t");
  // Serial.println(smoothed);
  // Serial.println("--------------------------");
}

void Motor::stop(byte smoothStep = SMOOTH_HARD) {
  go(0, smoothStep);
}

void Motor::tick() {
  int smoothed = smooth(_targetSpeed);
  setSpeed(smoothed);
}

int Motor::smooth(int speed) {
  if ((_smoothStep <= 0) || (speed == _currentSpeed)) {
    return speed;
  }

  if (millis() - _smoothTime < SMOOTH_PERIOD) {
    return _currentSpeed;
  }

  _smoothTime = millis();

  // Serial.print("_currentSpeed\t");
  // Serial.println(_currentSpeed);

  int delta = abs(speed - _currentSpeed);
  if (delta > _smoothStep) {
    delta = _smoothStep;
  }

  if (speed > _currentSpeed) {
    speed = _currentSpeed + delta;
  }
  else if (speed < _currentSpeed) {
    speed = _currentSpeed - delta;
  }

  speed = constrainSpeed(speed);
  return speed;
}

void Motor::setSpeed(int speed) {
  speed = constrainSpeed(speed);
  _currentSpeed = speed;

  bool forward = speed >= 0;
  speed = abs(speed);

  // левый и правый драйверы моторов выдают немного разное напряжение и из-за этого робот отклоняется влево
  // поэтому здесь немного обрезаем скорость для более быстрой правой стороны
  byte truncatedMaxSpeed = forward ? _truncatedForwardMaxSpeed : _truncatedBackMaxSpeed;
  speed = map(speed, -MAX_SPEED, MAX_SPEED + 1, -truncatedMaxSpeed, truncatedMaxSpeed + 1);

  digitalWrite(_inaPin, forward);
  digitalWrite(_inbPin, !forward);
  analogWrite(_pwmPin, speed);
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
