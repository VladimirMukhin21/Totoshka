#pragma once
#include <GyverPID.h>
#include "Hand.h"
#include "Color.h"
#define P 2.7
#define I 0.2
#define D 0.15
#define DT 100 // 2.7 0.2 0.15

class ProgRideTheLine {
  public:
    void init(Truck &truck, Hand &hand, Color &color);
    void start();
    void stop();
    void tick();
    bool isRunning();

  private:
    const int _driveSpeed = 40;
    //const byte _coef = 3;
    GyverPID _pid = GyverPID(P, I, D, DT);

    Truck* _truck;
    Hand* _hand;
    Color* _color;

    //#define DEBUG
#ifdef DEBUG
    unsigned long _printTime = millis();
#endif

    unsigned long _tickTime = millis();
    bool _isRunning = false;
};

void ProgRideTheLine::init(Truck &truck, Hand &hand, Color &color) {
  _truck = &truck;
  _hand = &hand;
  _color = &color;

  _pid.setpoint = 0;
  _pid.setMode(ON_RATE);
  _pid.setLimits(-255, 255);
}

void ProgRideTheLine::start() {
  if (!_isRunning) {
    //_hand->


    _color->enable();
    _isRunning = true;
  }
}

void ProgRideTheLine::stop() {
  _truck->stop();
  _hand->stop();
  _isRunning = false;
  _color->disable();
}

void ProgRideTheLine::tick() {
  if (!_isRunning) {
    return;
  }
  int l = _color->getLeft();
  int r = _color->getRight();

  _pid.input = r - l;
  //Serial.print(l - r);
  //int turn = (l - r) * _coef;
  //int turn = (int)((double)(l - r) * 3);
  int turn = _pid.getResult();
  //Serial.print("\t");
  //Serial.println((double)l / (double)r);
  //Serial.println(turn);
  _truck->goAndTurn(_driveSpeed, turn);
  /*Serial.print(l);
    Serial.print("\t");
    Serial.println(r);*/
  /*Serial.print("\t");
    Serial.print(turn);
    Serial.print("\t");
    Serial.println((double)l / (double)r);*/
}

bool ProgRideTheLine::isRunning() {
  return _isRunning;
}
