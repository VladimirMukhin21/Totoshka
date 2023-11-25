#pragma once

//#include <GyverPID.h>
#include "Truck.h"
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
    INIT_HAND,
    DRIVE,
    SLOW_DRIVE,
    TAKE,
    TIN_UP
  };

  const int _driveSpeed = 70;        // скорость подъезда к маяку
  const int _driveSlowSpeed = 40;    // скорость медленного подъезда к маяку когда маяк близко
  const int _distToSlowDrive = 150;  // расстояние до маяка, на котором снижаем скорость
  const int _distToTake = 75;        // расстояние до маяка, на котором останавливаемся и хватаем маяк
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
    _phase = INIT_HAND;
    //Serial.println("start");
  }
  else if (_phase == INIT_HAND) {
    // ждем пока рука опустится...
    //Serial.println("init hand");
    if (!_hand->isRunning()) {
      // рука опустилась => подъезжаем к маяку
      _distMeter->enable();
      int dist = _distMeter->getDist();
      if (dist > _distToSlowDrive) {
        _truck->goStraight(_driveSpeed);
        _phase = DRIVE;
      }
      else {
        _truck->goStraight(_driveSlowSpeed);
        _phase = SLOW_DRIVE;
      }
    }
  }
  else if (_phase == DRIVE) {
    // подъезжаем к маяку
    int dist = _distMeter->getDist();
    if (dist <= _distToSlowDrive) {
      // подъехали к маяку близко => снижаем скорость, девиацию не сбрасываем
      _truck->goStraight(_driveSlowSpeed, false);
      _phase = SLOW_DRIVE;
    }
  }
  else if (_phase == SLOW_DRIVE) {
    // медленно подъезжаем к маяку
    int dist = _distMeter->getDist();
    if (dist <= _distToTake) {
      // доехали до маяка
      _truck->stop(Motor::SMOOTH_OFF);
      _distMeter->disable();
      _hand->takeTin();
      _phase = TAKE;
    }
  }
  else if (_phase == TAKE) {
    // ждем пока рука сожмется...
    //Serial.println("take");
    if (!_hand->isRunning()) {
      // рука сжалась => поднимаем маяк
      _hand->tinUp();
      _phase = TIN_UP;
    }
  }
  else if (_phase == TIN_UP) {
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
