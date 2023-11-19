#pragma once

#include "Tail.h"

class ProgStairsUp {
  public:
    void init(Truck &truck, Tail &tail);
    void start();
    void stop();
    void tick();
    bool isRunning();

  private:
    enum Phase {
      STARTING,
      INIT_TAIL_UP,
      INIT_DRIVING,
      MOVING_TAIL_UP,
      MOVING_TAIL_DOWN,
      DRIVING_BOW_UP,
      DRIVING_STERN_UP,
      FINISH_DRIVING,
      STOPPING
    };

    const byte _stairsCount = 5;
    const byte _tailUpDeg = 125;
    const byte _tailDownDeg = 15;
    const int _driveSpeed = 180;
    const int _driveToStairsTime = 1400; //1000
    const int _bowUpTime = 1400; // 1000
    const int _sternUpTime = 900; // 500
    const int _finishDriveTime = 150;

    Truck* _truck;
    Tail* _tail;
    Phase _phase;
    byte _passedStairs;

    //#define DEBUG
#ifdef DEBUG
    unsigned long _printTime = millis();
#endif

    unsigned long _tickTime = millis();
    bool _isRunning = false;
};

void ProgStairsUp::init(Truck &truck, Tail &tail) {
  _truck = &truck;
  _tail = &tail;
}

void ProgStairsUp::start() {
  if (!_isRunning) {
    _phase = STARTING;
    _passedStairs = 0;
    _isRunning = true;
  }
}

void ProgStairsUp::stop() {
  _truck->stop();
  _tail->stop();
  _isRunning = false;
}

void ProgStairsUp::tick() {
  if (!_isRunning) {
    return;
  }

  switch (_phase) {
    case STARTING:
      // программа стартует => поднимаем хвост в нач. положение
      _tail->moveTo(_tailUpDeg);
      _phase = INIT_TAIL_UP;
      break;

    case INIT_TAIL_UP:
      if (!_tail->isRunning()) {
        // хвост поднялся => подъезжаем к лестнице
        _truck->autoGo(_driveSpeed, _driveToStairsTime);
        _phase = INIT_DRIVING;
      }
      break;

    case INIT_DRIVING:
      if (!_truck->isRunning()) {
        // подъехали к лестнице => забрасываем нос
        _truck->autoGo(_driveSpeed, _bowUpTime);
        _phase = DRIVING_BOW_UP;
      }
      break;

    case DRIVING_BOW_UP:
      if (!_truck->isRunning()) {
        // забросили нос => опускаем хвост
        _tail->moveTo(_tailDownDeg);
        _phase = MOVING_TAIL_DOWN;
      }
      break;

    case MOVING_TAIL_DOWN:
      if (!_tail->isRunning()) {
        // опустили хвост => забрасываем корму
        _truck->autoGo(_driveSpeed, _sternUpTime);
        _phase = DRIVING_STERN_UP;
      }
      break;

    case DRIVING_STERN_UP:
      if (!_truck->isRunning()) {
        // забросили корму => поднимаем хвост
        _tail->moveTo(_tailUpDeg);
        _phase = MOVING_TAIL_UP;
      }
      break;

    case MOVING_TAIL_UP:
      if (!_tail->isRunning()) {
        // хвост поднялся => забрасываем нос ИЛИ отъезжаем от ступеней
        _passedStairs++;

        if (_passedStairs < _stairsCount) {
          // еще не все ступени проехали => забрасываем нос
          _truck->autoGo(_driveSpeed, _bowUpTime);
          _phase = DRIVING_BOW_UP;
        }
        else {
          // проехали все ступени => отъезжаем от ступеней
          _truck->autoGo(_driveSpeed, _finishDriveTime);
          _phase = FINISH_DRIVING;
          return;
        }
      }
      break;

    case FINISH_DRIVING:
      if (!_truck->isRunning()) {
        // отъехали от ступеней => стоп
        _phase = STOPPING;
      }
      break;

    case STOPPING:
      stop();
      break;
  }
}

bool ProgStairsUp::isRunning() {
  return _isRunning;
}
