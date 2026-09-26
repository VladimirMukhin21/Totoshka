#pragma once

#include "Truck.h"

class ProgBoard {
public:
  void init(Truck &truck);
  void start();
  void stop();
  void tick();
  bool isRunning();

private:
  enum Phase {
    NONE,
    STARTING,
    DRIVE_TOP,
    WAIT,
    STOP
  };

  const int _upDriveSpeed = 100;
  const int _downDriveSpeed = 255;
  const int _waitTime = 3000;
  unsigned long _lastTime = millis();

  Truck *_truck;
  Phase _phase = NONE;
};

void ProgBoard::init(Truck &truck) {
  _truck = &truck;
}

void ProgBoard::start() {
  if (isRunning()) {
    return;
  }

  _phase = STARTING;
}

void ProgBoard::stop() {
  _truck->stop();
  _phase = NONE;
}

void ProgBoard::tick() {
  if (_phase == NONE) {
    return;
  }
  else if (_phase == STARTING) {
    // программа стартует => заезжаем на доску
    _truck->goHillUp(_upDriveSpeed, 6000, 1800, 30);
    _phase = DRIVE_TOP;
  }
  else if (_phase == DRIVE_TOP) {
    if (!_truck->isRunning()) {
      // на доску заехали => стоим _waitTime сек
      _truck->stop();
      _phase = WAIT;
    }
  }
  else if (_phase == WAIT) {
    if (!_truck->isRunning()) {
      // подождали _waitTime сек => спускаемся с доски
      if (millis() - _lastTime < _waitTime) {
        return;
      }
      _truck->goHillDown(_downDriveSpeed, -4000, -1000, 50);
      _phase = STOP;
    }
  }
  else if (_phase == STOP) {
    if (!_truck->isRunning()) {
      // спустились => стоп
      stop();
    }
  }

  _lastTime = millis();
}

bool ProgBoard::isRunning() {
  return _phase != NONE;
}
