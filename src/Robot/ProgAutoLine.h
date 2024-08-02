#pragma once

#include <GyverPID.h>
#include "Hand.h"
#include "Color.h"

class ProgAutoLine {
public:
  void init(Truck &truck, Hand &hand, Color &color);
  void start();
  void stop();
  void tick();
  bool isRunning();

private:
  const int _driveSpeed = 65;                     // 70
  GyverPID _pid = GyverPID(1.8, 0, 0.1, 100);  //   4.5, 0.1, 0.3, 100      3.1, 0.15, 0.25, 100     // 2.0, 0, 0.2, 100  //   2.7, 0.2, 0.15, 100   4, 0.25, 0.2, 100   3.5, 0.2, 0.35, 100

  Truck *_truck;
  Hand *_hand;
  Color *_color;

  unsigned long _tickTime = millis();
  bool _isRunning = false;
};

void ProgAutoLine::init(Truck &truck, Hand &hand, Color &color) {
  _truck = &truck;
  _hand = &hand;
  _color = &color;

  _pid.setpoint = 0;
  _pid.setMode(ON_RATE);
  _pid.setDirection(REVERSE);
  _pid.setLimits(-255, 255);
}

void ProgAutoLine::start() {
  if (!_isRunning) {
    _color->enable();
    _isRunning = true;
  }
}

void ProgAutoLine::stop() {
  _truck->stop();
  _hand->stop();
  _isRunning = false;
  _color->disable();
}

void ProgAutoLine::tick() {
  if (!_isRunning) {
    return;
  }

  int l = _color->getLeft();
  int r = _color->getRight();

  _pid.input = l - r;
  int turn = _pid.getResult();  // ПИД-регулятор
  //int turn = (int)((double)(l - r) * 3); // П-регулятор

  _truck->goAndTurn(_driveSpeed, turn, Motor::SMOOTH_OFF);

  /*Serial.print(l);
    Serial.print("\t");
    Serial.println(r);
    Serial.print("\t");
    Serial.print(l - r);
    Serial.print("\t");
    Serial.print(turn);
    Serial.print("\t");
    Serial.println((double)l / (double)r);*/
}

bool ProgAutoLine::isRunning() {
  return _isRunning;
}
