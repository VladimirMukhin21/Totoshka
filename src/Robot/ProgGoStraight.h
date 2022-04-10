#pragma once

#include "Gyro.h"

class ProgGoStraight {
  public:
    void init(Truck &truck, Gyro &gyro);
    void start();
    void stop();
    void tick();
    bool isRunning();

  private:
    const int _driveSpeed = 150;

    Truck* _truck;
    Gyro* _gyro;
    long _deviation = 0;

    bool _isRunning = false;
};

void ProgGoStraight::init(Truck &truck, Gyro &gyro) {
  _truck = &truck;
  _gyro = &gyro;
}

void ProgGoStraight::start() {
  if (!_isRunning) {
    _deviation = 0;
    _isRunning = true;
  }
}

void ProgGoStraight::stop() {
  _truck->stop();
  _isRunning = false;
}

void ProgGoStraight::tick() {
  if (!_isRunning) {
    return;
  }

  _deviation += _gyro->getCourse();

  // запомнили курс => начинаем движение
  if (_deviation > 0)
    _truck->speedGo(_driveSpeed, _driveSpeed + 30);
  else if (_deviation < 0)
    _truck->speedGo(_driveSpeed + 30, _driveSpeed);
  else
    _truck->speedGo(_driveSpeed, _driveSpeed);
}

bool ProgGoStraight::isRunning() {
  return _isRunning;
}
