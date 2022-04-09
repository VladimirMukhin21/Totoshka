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
    enum Phase {
      STARTING,
      INIT_DRIVING,
      STOPPING
    };

    const int _driveSpeed = 150;

    Truck* _truck;
    Gyro* _gyro;
    Phase _phase;
    int _course;

    bool _isRunning = false;

    int getCourse();
};

void ProgGoStraight::init(Truck &truck, Gyro &gyro) {
  _truck = &truck;
  _gyro = &gyro;
  _course = getCourse();
}

void ProgGoStraight::start() {
  if (!_isRunning) {
    _phase = STARTING;
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

  switch (_phase) {
    case STARTING:
      // программа стартует => запоминаем курс
      _course = getCourse();
      _phase = INIT_DRIVING;
      break;

    case INIT_DRIVING:
      // запомнили курс => начинаем движение
      if (getCourse() > _course)
        _truck->speedGo(_driveSpeed, _driveSpeed + 30);
      else if (getCourse() < _course)
        _truck->speedGo(_driveSpeed + 30, _driveSpeed);
      else
        _truck->speedGo(_driveSpeed, _driveSpeed);

      _phase = INIT_DRIVING;

    case STOPPING:
      stop();
      break;
  }
}

bool ProgGoStraight::isRunning() {
  return _isRunning;
}

int ProgGoStraight::getCourse() {
  return mpu.getRotationX(); // ------------------------------------
}
