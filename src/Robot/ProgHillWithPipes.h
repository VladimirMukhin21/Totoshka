#pragma once

#include "Truck.h"
#include "Tail.h"

class ProgHillWithPipes {
public:
  void init(Truck &truck, Tail &tail);
  void start();
  void stop();
  void tick();
  bool isRunning();

private:
  enum Phase {
    NONE,
    STARTING,
    INIT_TAIL_UP,
    DRIVING_BOW_UP,
    TAIL_DOWN,
    DRIVING_STERN_UP,
    TAIL_HORIZ,
    DRIVE_DOWN,
    TAIL_UP,
    FINISH_DRIVING
  };

  const byte _tailUpDeg = 125;
  const byte _tailHorizDeg = 80;
  const byte _tailDownDeg = 15;
  const int _driveSpeed = 180;
  const int _slowDriveSpeed = 40;

  Truck *_truck;
  Tail *_tail;
  Phase _phase = NONE;
};

void ProgHillWithPipes::init(Truck &truck, Tail &tail) {
  _truck = &truck;
  _tail = &tail;
}

void ProgHillWithPipes::start() {
  if (isRunning()) {
    return;
  }

  _phase = STARTING;
}

void ProgHillWithPipes::stop() {
  _truck->stop();
  _tail->stop();
  _phase = NONE;
}

void ProgHillWithPipes::tick() {
  if (_phase == NONE) {
    return;
  }
  else if (_phase == STARTING) {
    // программа стартует => поднимаем хвост в нач. положение
    _tail->moveTo(_tailUpDeg);
    _phase = INIT_TAIL_UP;
  }
  else if (_phase == INIT_TAIL_UP) {
    if (!_tail->isRunning()) {
      // хвост поднялся => подъезжаем пока не встанем на дыбы
      _truck->goWhilePitchInRange(_driveSpeed, -8000, 8000, false, 2000);
      _phase = DRIVING_BOW_UP;
    }
  }
  else if (_phase == DRIVING_BOW_UP) {
    if (!_truck->isRunning()) {
      // встали на дыбы => опускаем хвост
      _tail->moveTo(_tailDownDeg);
      _phase = TAIL_DOWN;
    }
  }
  else if (_phase == TAIL_DOWN) {
    if (!_tail->isRunning()) {
      // опустили хвост => даем газ, забрасываем корму
      _truck->goWhilePitchInRange(_driveSpeed, -4000, 8000, false, 2000);
      _phase = DRIVING_STERN_UP;
    }
  }
  else if (_phase == DRIVING_STERN_UP) {
    if (!_truck->isRunning()) {
      // забросили корму => ставим хвост горизонтально для смещения ЦМ назад
      _tail->moveTo(_tailHorizDeg);
      _phase = TAIL_HORIZ;
    }
  }
  else if (_phase == TAIL_HORIZ) {
    if (!_tail->isRunning()) {
      // хвост в горизонтали => на маленькой скорости съезжаем прямо
      _truck->goWhilePitchInRange(_slowDriveSpeed, -6000, 2000, false, 2000);
      _phase = DRIVE_DOWN;
    }
  }
  else if (_phase == DRIVE_DOWN) {
    if (!_truck->isRunning()) {
      // съехали носом => поднимаем хвост
      _tail->moveTo(_tailUpDeg);
      _phase = TAIL_UP;
    }
  }
  else if (_phase == TAIL_UP) {
    if (!_tail->isRunning()) {
      // хвост поднялся => включаем скорость и едем до горизонтали
      _truck->goWhilePitchInRange(_slowDriveSpeed, -10000, 1000, false, 2000);
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

bool ProgHillWithPipes::isRunning() {
  return _phase != NONE;
}
