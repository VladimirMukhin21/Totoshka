#pragma once

#include "Truck.h"

class ProgTruncatedPyramid {
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
    //DRIVE_TO_FIX,
    DRIVE_TOP,
    TURN,
    DRIVE_DOWN,
    FINISH_DRIVING
  };

  const int _driveSpeed = 100;
  const int _slowDriveSpeed = 80;

  Truck *_truck;
  Phase _phase = NONE;
  int _turnOnTopAngleDeg = 0;
};

void ProgTruncatedPyramid::init(Truck &truck) {
  _truck = &truck;
}

void ProgTruncatedPyramid::start(int turnOnTopAngleDeg) {
  if (isRunning()) {
    return;
  }

  _turnOnTopAngleDeg = turnOnTopAngleDeg;
  _phase = STARTING;
}

void ProgTruncatedPyramid::stop() {
  _truck->stop();
  _phase = NONE;
}

void ProgTruncatedPyramid::tick() {
  if (_phase == NONE) {
    return;
  }
  else if (_phase == STARTING) {
    // программа стартует => заезжаем на вершину
    _truck->goHillUp(_driveSpeed);
    _phase = DRIVE_TOP;
    // _phase = DRIVE_TO_FIX;
  }
  /*else if (_phase == DRIVE_TO_FIX) {
    if (!_truck->isRunning()) {
      // заехали на вершину => еще чуть-чуть подъезжаем, чтобы закрепиться
      _truck->autoGo(_driveSpeed, 100);
      _phase = DRIVE_TOP;
    }
  }*/
  else if (_phase == DRIVE_TOP) {
    if (!_truck->isRunning()) { 
      // закрепились => поворачиваем
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
      // съехали => еще немного отъезжаем
      _truck->autoGo(_driveSpeed, 1000);
      _phase = FINISH_DRIVING;
    }
  }
  else if (_phase == FINISH_DRIVING) {
    if (!_truck->isRunning()) {
      // отъехали => стоп
      stop();
    }
  }
}

bool ProgTruncatedPyramid::isRunning() {
  return _phase != NONE;
}
