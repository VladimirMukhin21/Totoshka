#pragma once

#include "Truck.h"
#include "Tail.h"
#include "Hand.h"
#include "DistMeter.h"

class ProgHillWithPipesAutoTin {
public:
  void init(Truck &truck, Tail &tail, Hand &hand, DistMeter &distMeter);
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
    INIT_HAND,
    DRIVE,
    TAKE,
    TIN_UP,
    DRIVING_STERN_UP,
    DRIVE_TO_CENTER,
    TAIL_HORIZ,
    DRIVE_TO_CENTER2,
    TURN,
    DRIVE_DOWN,
    TAIL_UP,
    FINISH_DRIVING,
    STOP
  };

  const byte _tailInitialDeg = 125;
  const byte _tailUpDeg = 139;
  const byte _tailHorizDeg = 80;
  const byte _tailDownDeg = 15;
  const int _driveSpeed = 230;  //180;
  const int _slowDriveSpeed = 50;
  const int _distToTake = 65;  // 75

  const byte _shoulderInitPos = 60;
  const byte _elbowInitPos = 30;
  const byte _rotateCenterPos = 110;
  const byte _clawInitPos = 105;

  const byte _border = 10; // порог расстояния, при котором берем маяк, защита от -1 (ошибка: датчик не инициализировался)

  const byte _shoulderMaxTakeTinPos = 100;
  const byte _elbowMaxTakeTinPos = 76;
  const byte _shoulderInitSpeed = 1;
  const byte _elbowInitSpeed = 1;

  const byte _shoulderHandToBackPos = 30;
  const byte _elbowHandToBackPos = 109;
  const byte _shoulderToBackSpeed = 4;
  const byte _elbowToBackSpeed = 3;

  Truck *_truck;
  Tail *_tail;
  Hand *_hand;
  DistMeter *_distMeter;
  Phase _phase = NONE;
  int _turnOnTopAngleDeg = 0;
};

void ProgHillWithPipesAutoTin::init(Truck &truck, Tail &tail, Hand &hand, DistMeter &distMeter) {
  _truck = &truck;
  _tail = &tail;
  _hand = &hand;
  _distMeter = &distMeter;
}

void ProgHillWithPipesAutoTin::start(int turnOnTopAngleDeg) {
  if (isRunning()) {
    return;
  }

  _turnOnTopAngleDeg = turnOnTopAngleDeg;
  _phase = STARTING;
}

void ProgHillWithPipesAutoTin::stop() {
  _truck->stop();
  _tail->stop();
  _hand->stop();
  //_distMeter->disable(); // было раскомментировано
  _phase = NONE;
}

void ProgHillWithPipesAutoTin::tick() {
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
      // опустили хвост => опускаем руку в положение для захвата маяка
      _hand->handToPos(_shoulderInitPos, _elbowInitPos, _rotateCenterPos, _clawInitPos);  // ставить руку в другое положение, увеличить скорость
      _phase = INIT_HAND;
    }
  }
  else if (_phase == INIT_HAND) {
    if (!_hand->isRunning()) {
      // рука опустилась => подъезжаем к маяку
      _distMeter->enable();
      int dist = _distMeter->getDist();
      if ((dist <= _distToTake) && (dist > _border)) {
        _hand->takeTin();
        _phase = TAKE;
      }
      else {
        _hand->handToPos(_shoulderMaxTakeTinPos, _elbowMaxTakeTinPos, -1, -1, _shoulderInitSpeed, _elbowInitSpeed);
        _phase = DRIVE;
      }
    }
  }
  else if (_phase == DRIVE) {
    // медленно опускаем руку
    int dist = _distMeter->getDist();
    if (dist <= _distToTake) {
      // опустили руку
      _hand->stop();
      _distMeter->disable();  // было закомментировано
      _hand->takeTin();
      _phase = TAKE;
    }
  }
  else if (_phase == TAKE) {
    // ждем пока рука сожмется...
    //Serial.println("take");
    if (!_hand->isRunning()) {
      // рука сжалась => поднимаем маяк
      _hand->handToPos(_shoulderHandToBackPos, _elbowHandToBackPos, -1, -1, _shoulderToBackSpeed, _shoulderToBackSpeed);
      _phase = TIN_UP;
    }
  } 
  else if (_phase == TIN_UP) {
    // ждем пока рука с маяком поднимается...
    if (!_hand->isRunning()) {
      // хвост опущен, рука с банкой на спине => даем газ, забрасываем корму
      _truck->goWhilePitchInRange(255, -4000, 12000, false, 5000);
      _phase = DRIVING_STERN_UP;
    }
  }
  else if (_phase == DRIVING_STERN_UP) {
    if (!_truck->isRunning()) {
      // забросили корму => еще немного подъезжаем до центра
      _truck->autoGo(_driveSpeed, 300);
      _tail->moveTo(_tailHorizDeg);
      _phase = DRIVE_TO_CENTER;
    }
  }
  else if (_phase == DRIVE_TO_CENTER) {
    if (!_truck->isRunning()) {
      // подъехали до центра => ставим хвост горизонтально для смещения ЦМ назад
      // _tail->moveTo(_tailHorizDeg);
      _truck->autoGo(_driveSpeed, 150);
      // _phase = TAIL_HORIZ;
      _phase = DRIVE_TO_CENTER2;
    }
  }
  // else if (_phase == TAIL_HORIZ) {
  //   if (!_tail->isRunning()) {
  //     // хвост в горизонтали => //поворачиваем
  //     _truck->autoGo(_driveSpeed, 150);
  //     _phase = DRIVE_TO_CENTER2;
  //   }
  // }

  else if (_phase == DRIVE_TO_CENTER2) {
    // подъехали до центра => ставим хвост горизонтально для смещения ЦМ назад
    if (!_tail->isRunning()) {
      _truck->turn(_turnOnTopAngleDeg);
      _phase = TURN;
    }
  }

  else if (_phase == TURN) {
    if (!_truck->isRunning()) {
      // повернули => на маленькой скорости съезжаем прямо
      _truck->goWhilePitchInRange(_slowDriveSpeed, -8000, 5000, false, 5000);
      _phase = DRIVE_DOWN;
    }
  }
  else if (_phase == DRIVE_DOWN) {
    if (!_truck->isRunning()) {
      // съехали носом => поднимаем хвост
      _tail->moveTo(_tailUpDeg);
      _truck->goWhilePitchInRange(_driveSpeed, -20000, -1000, false, 5000);
      // _phase = TAIL_UP;
      _phase = FINISH_DRIVING;
    }
  }
  // else if (_phase == TAIL_UP) {
  //   if (!_tail->isRunning()) {
  //     // хвост поднялся => включаем скорость и едем до горизонтали
  //     _truck->goWhilePitchInRange(_driveSpeed, -20000, -1000, false, 5000);
  //     _phase = FINISH_DRIVING;
  //   }
  // }
  else if (_phase == FINISH_DRIVING) {
    if (!_tail->isRunning()) {
      // доехали до горизонтали => еще немного отъезжаем
      _truck->autoGo(_driveSpeed, 400);
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

bool ProgHillWithPipesAutoTin::isRunning() {
  return _phase != NONE;
}
