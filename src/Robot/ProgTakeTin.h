#pragma once

//#include <GyverPID.h>
#include "Hand.h"
#include "DistMeter.h"

class ProgTakeTin {
public:
  void init(Truck &truck, Hand &hand, DistMeter &distMeter);
  void start();
  void stop();
  void tick();
  bool isRunning();

private:
  enum Phase {
    NONE,
    START,
    HAND_DOWN,
    DRIVE,
    TAKE,
    HAND_UP
  };

  const int _driveSpeed = 70;
  const int _distToTake = 70;
  //GyverPID _pid = GyverPID(3.6, 0.15, 0.2, 100);

  Truck *_truck;
  Hand *_hand;
  DistMeter *_distMeter;
  Phase _phase = NONE;

  unsigned long _tickTime = millis();
};

void ProgTakeTin::init(Truck &truck, Hand &hand, DistMeter &distMeter) {
  _truck = &truck;
  _hand = &hand;
  _distMeter = &distMeter;

  /*_pid.setpoint = 0;
    _pid.setMode(ON_RATE);
    _pid.setDirection(REVERSE);
    _pid.setLimits(-255, 255);*/
}

void ProgTakeTin::start() {
  if (!isRunning()) {
    _phase = START;
  }
}

void ProgTakeTin::stop() {
  _truck->stop();
  _hand->stop();
  _distMeter->disable();
  _phase = NONE;
  //Serial.println("stop");
}

void ProgTakeTin::tick() {
  if (!isRunning()) {
    return;
  }

  if (_phase == START) {
    // программа стартует => опускаем руку в нач. положение
    _hand->handToTakeTin();
    _phase = HAND_DOWN;
    //Serial.println("start");
  } else if (_phase == HAND_DOWN) {
    // ждем пока рука опустится...
    //Serial.println("init hand");
    if (!_hand->isRunning()) {
      // рука опустилась => подъезжаем к маяку
      _distMeter->enable();
      _truck->goStraight(_driveSpeed);
      _phase = DRIVE;
    }
  } else if (_phase == DRIVE) {
    // подъезжаем к маяку
    int dist = _distMeter->getDist();
    //Serial.print("drive ");
    //Serial.println(dist);
    if (dist <= _distToTake) {
      // доехали до маяка
      _truck->stop();
      _distMeter->disable();
      _hand->takeTin();
      _phase = TAKE;
    }
  } else if (_phase == TAKE) {
    // ждем пока рука сожмется...
    //Serial.println("take");
    if (!_hand->isRunning()) {
      // рука сжалась => поднимаем маяк
      _hand->handUp();
      _phase = HAND_UP;
    }
  } else if (_phase == HAND_UP) {
    // ждем пока рука с маяком поднимается...
    //Serial.println("hand up");
    if (!_hand->isRunning()) {
      // маяк подняли => стоп
      stop();
    }
  }
}

bool ProgTakeTin::isRunning() {
  return _phase != NONE;
}
