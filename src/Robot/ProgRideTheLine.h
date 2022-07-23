#pragma once
#include "Hand.h"
#include "Color.h"

class ProgRideTheLine {
  public:
    void init(Truck &truck, Hand &hand, Color &color);
    void start();
    void stop();
    void tick();
    bool isRunning();

  private:
    const int _driveSpeed = 40;
    const byte _coef = 3;

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
}

void ProgRideTheLine::start() {
  if (!_isRunning) {
    //_hand->
    _isRunning = true;
  }
}

void ProgRideTheLine::stop() {
  _truck->stop();
  _hand->stop();
  _isRunning = false;
}

void ProgRideTheLine::tick() {
  if (!_isRunning) {
    return;
  }
  int l = _color->getLeft();
  int r = _color->getRight();
  int turn = (l - r) * _coef;
  _truck->goAndTurn(_driveSpeed, turn);
  /*Serial.print(l);
  Serial.print("\t");
  Serial.print(r);
  Serial.print("\t");
  Serial.println(turn);*/
}

bool ProgRideTheLine::isRunning() {
  return _isRunning;
}
