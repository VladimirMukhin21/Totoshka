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
      INIT_HAND,
      DRIVE,
      TAKE,
      HAND_UP
    };

    const int _driveSpeed = 50;
    const int _minDist = 50;
    //GyverPID _pid = GyverPID(3.6, 0.15, 0.2, 100);

    Truck* _truck;
    Hand* _hand;
    DistMeter* _distMeter;
    Phase _phase;

    unsigned long _tickTime = millis();
};

void ProgTakeTin::init(Truck &truck, Hand &hand, DistMeter &distMeter) {
  _truck = &truck;
  _hand = &hand;
  _distMeter = &distMeter;
  _phase = NONE;

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

  switch (_phase) {
    case START:
      // программа стартует => опускаем руку в нач. положение
      _hand->handToTakeTin();
      _phase = INIT_HAND;
      //Serial.println("start");
      break;

    case INIT_HAND:
      // ждем пока рука опустится...
      //Serial.println("init hand");
      if (!_hand->isRunning()) {
        // рука опустилась => подъезжаем к маяку
        _distMeter->enable();
        _phase = DRIVE;
      }
      break;

    case DRIVE:
      // подъезжаем к маяку
      int dist = _distMeter->getDist();
      //Serial.print("drive ");
      //Serial.println(dist);
      if (dist > _minDist) {
        _truck->goAndTurn(_driveSpeed, 0);
      }
      else {
        // доехали до маяка
        _truck->goAndTurn(0, 0);
        _hand->takeTin();
        _distMeter->disable();
        _phase = TAKE;
      }
      break;

    case TAKE:
      // ждем пока рука сожмется...
      //Serial.println("take");
      if (!_hand->isRunning()) {
        // рука сжалась => поднимаем маяк
        _hand->handUp();
        _phase = HAND_UP;
      }
      break;

    case HAND_UP:
      // ждем пока рука с маяком поднимается...
      //Serial.println("hand up");
      if (!_hand->isRunning()) {
        // маяк подняли => стоп
        stop();
      }
      break;
  }
}

bool ProgTakeTin::isRunning() {
  return _phase != NONE;
}
