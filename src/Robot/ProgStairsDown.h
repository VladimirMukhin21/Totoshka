#pragma once

#include "Tail.h"

class ProgStairsDown {
  public:
    void init(Truck &truck, Tail &tail);
    void start();
    void stop();
    void tick();
    bool isRunning();

  private:
    enum Phase {
      STARTING,
      INIT_DRIVING,
      MOVING_TAIL_DOWN,
      DRIVING_STERN_DOWN,
      MOVING_TAIL_UP,
      DRIVING_BOW_DOWN,
      FINISH_DRIVING,
      STOPPING
    };

    const byte _stairsCount = 5;
    const byte _tailUpDeg = 125;
    const byte _tailDownDeg = 15;
    const int _driveSpeed = -50;
    const int _driveToStairsTime = 1000; //500
    const int _bowDownTime = 1250; //500
    const int _sternDownTime = 750; //250
    const int _finishDriveTime = 500; //500

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

void ProgStairsDown::init(Truck &truck, Tail &tail) {
  _truck = &truck;
  _tail = &tail;
}

void ProgStairsDown::start() {
  if (!_isRunning) {
    _phase = STARTING;
    _passedStairs = 0;
    _isRunning = true;
  }
}

void ProgStairsDown::stop() {
  _truck->stop();
  _tail->stop();
  _isRunning = false;
}

void ProgStairsDown::tick() {
  if (!_isRunning) {
    return;
  }

  switch (_phase) {
    case STARTING:
      // программа стартует => подъезжаем к лестнице
      _truck->autoGo(_driveSpeed, _driveToStairsTime);
      _phase = INIT_DRIVING;
      break;

    case INIT_DRIVING:
      if (!_truck->isRunning()) {
        // подъехали к лестнице => опускаем хвост
        _tail->moveTo(_tailDownDeg);
        _phase = MOVING_TAIL_DOWN;
      }
      break;

    case MOVING_TAIL_DOWN:
      if (!_tail->isRunning()) {
        // опустили хвост => опускаем корму
        _truck->autoGo(_driveSpeed, _sternDownTime);
        _phase = DRIVING_STERN_DOWN;
      }
      break;

    case DRIVING_STERN_DOWN:
      if (!_truck->isRunning()) {
        // опустили корму => поднимаем хвост
        _tail->moveTo(_tailUpDeg);
        _phase = MOVING_TAIL_UP;
      }
      break;

    case MOVING_TAIL_UP:
      if (!_tail->isRunning()) {
        // хвост поднялся => опускаем нос
        _truck->autoGo(_driveSpeed, _bowDownTime);
        _phase = DRIVING_BOW_DOWN;
      }
      break;

    case DRIVING_BOW_DOWN:
      if (!_truck->isRunning()) {
        // опустили нос => опускаем хвост ИЛИ отъезжаем от ступеней
        _passedStairs++;

        if (_passedStairs < _stairsCount) {
          // еще не все ступени проехали => опускаем хвост
          _tail->moveTo(_tailDownDeg);
          _phase = MOVING_TAIL_DOWN;
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

bool ProgStairsDown::isRunning() {
  return _isRunning;
}
