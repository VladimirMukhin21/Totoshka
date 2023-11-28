#pragma once

#include "Truck.h"
#include "Tail.h"

class ProgHillWithPipes {
public:
  void init(Truck &truck, Tail &tail);
  void start(int turnOnTopAngleDeg);
  void stop();
  void tick();
  bool isRunning();

private:
  enum Phase {
    NONE,
    STARTING,
    INIT_TAIL_UP,
    DRIVING_BOW_UP,
    DRIVE_TO_FIX,
    TAIL_DOWN,
    DRIVING_STERN_UP,
    DRIVE_TO_CENTER,
    TAIL_HORIZ,
    TURN,
    DRIVE_DOWN,
    TAIL_UP,
    FINISH_DRIVING,
    STOP
  };

  const byte _tailInitialDeg = 125;
  const byte _tailUpDeg = 145;
  const byte _tailHorizDeg = 80;
  const byte _tailDownDeg = 15;
  const int _driveSpeed = 180;
  const int _slowDriveSpeed = 40;

  Truck *_truck;
  Tail *_tail;
  Phase _phase = NONE;
  int _turnOnTopAngleDeg = 0;
};

void ProgHillWithPipes::init(Truck &truck, Tail &tail) {
  _truck = &truck;
  _tail = &tail;
}

void ProgHillWithPipes::start(int turnOnTopAngleDeg) {
  if (isRunning()) {
    return;
  }

  _turnOnTopAngleDeg = turnOnTopAngleDeg;
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
    _tail->moveTo(_tailInitialDeg);
    _phase = INIT_TAIL_UP;
  }
  else if (_phase == INIT_TAIL_UP) {
    if (!_tail->isRunning()) {
      // хвост поднялся => подъезжаем пока не встанем на дыбы
      _truck->goWhilePitchInRange(_driveSpeed, -8000, 10000, false, 5000);
      _phase = DRIVING_BOW_UP;
    }
  }
  else if (_phase == DRIVING_BOW_UP) {
    if (!_truck->isRunning()) {
      // встали на дыбы => еще чуть-чуть подъезжаем, чтобы закрепиться
      _truck->autoGo(_driveSpeed, 500);
      _phase = DRIVE_TO_FIX;
    }
  }
  else if (_phase == DRIVE_TO_FIX) {
    if (!_truck->isRunning()) {
      // закрепились => опускаем хвост
      _tail->moveTo(_tailDownDeg);
      _phase = TAIL_DOWN;
    }
  }
  else if (_phase == TAIL_DOWN) {
    if (!_tail->isRunning()) {
      // опустили хвост => даем газ, забрасываем корму
      _truck->goWhilePitchInRange(255, -4000, 8000, false, 5000);
      _phase = DRIVING_STERN_UP;
    }
  }
  else if (_phase == DRIVING_STERN_UP) {
    if (!_truck->isRunning()) {
      // забросили корму => еще немного подъезжаем до центра
      _truck->autoGo(_driveSpeed, 50);
      _phase = DRIVE_TO_CENTER;
    }
  }
  else if (_phase == DRIVE_TO_CENTER) {
    if (!_truck->isRunning()) {
      // подъехали до центра => ставим хвост горизонтально для смещения ЦМ назад
      _tail->moveTo(_tailHorizDeg);
      _phase = TAIL_HORIZ;
    }
  }
  else if (_phase == TAIL_HORIZ) {
    if (!_tail->isRunning()) {
      // хвост в горизонтали => поворачиваем
      _truck->turn(_turnOnTopAngleDeg);
      _phase = TURN;
    }
  }
  else if (_phase == TURN) {
    if (!_truck->isRunning()) {
      // повернули => на маленькой скорости съезжаем прямо
      _truck->goWhilePitchInRange(_slowDriveSpeed, -6000, 2000, false, 5000);
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
      _truck->goWhilePitchInRange(_driveSpeed, -20000, -1000, false, 5000);
      _phase = FINISH_DRIVING;
    }
  }
  else if (_phase == FINISH_DRIVING) {
    if (!_tail->isRunning()) {
      // доехали до горизонтали => еще немного отъезжаем
      _truck->autoGo(_driveSpeed, 1000);
      _phase = STOP;
    }
  }
  else if (_phase == STOP) {
    if (!_truck->isRunning()) {
      // отъехали => стоп
      stop();
    }
  }
}

bool ProgHillWithPipes::isRunning() {
  return _phase != NONE;
}
