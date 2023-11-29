#pragma once

#include "Truck.h"

class ProgHoof {
public:
  void init(Truck &truck);
  void start(int turnOnTopAngleDeg);
  void stop();
  void tick();
  bool isRunning();

private:
  enum Phase {
    NONE,
    STARTING,
    DRIVE_TOP,
    TURN,
    DRIVE_DOWN
  };

  const int _driveSpeed = 100;
  const int _slowDriveSpeed = 40;

  Truck *_truck;
  Phase _phase = NONE;
  int _turnOnTopAngleDeg = 0;
};

void ProgHoof::init(Truck &truck) {
  _truck = &truck;
}

void ProgHoof::start(int turnOnTopAngleDeg) {
  if (isRunning()) {
    return;
  }

  _turnOnTopAngleDeg = turnOnTopAngleDeg;
  _phase = STARTING;
}

void ProgHoof::stop() {
  _truck->stop();
  _phase = NONE;
}

void ProgHoof::tick() {
  if (_phase == NONE) {
    return;
  }
  else if (_phase == STARTING) {
    // программа стартует => заезжаем на вершину
    _truck->goHillUp(_driveSpeed, 4000, 800, 30);
    _phase = DRIVE_TOP;
  }
  else if (_phase == DRIVE_TOP) {
    if (!_truck->isRunning()) {
      // на вершину заехали => поворачиваем
      _truck->turn(_turnOnTopAngleDeg);
      _phase = TURN;
    }
  }
  else if (_phase == TURN) {
    if (!_truck->isRunning()) {
      // повернули => на маленькой скорости съезжаем прямо
      _truck->goHillDown(_slowDriveSpeed);
      _phase = DRIVE_DOWN;
    }
  }
  else if (_phase == DRIVE_DOWN) {
    if (!_truck->isRunning()) {
      // отъехали => стоп
      stop();
    }
  }
}

bool ProgHoof::isRunning() {
  return _phase != NONE;
}
