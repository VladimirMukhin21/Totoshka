#pragma once

#include "Gyro.h"

class ProgGoStraight {
  public:
    void init(Truck &truck, Gyro &gyro);
    void start();
    void stop();
    void tick();
    bool isRunning();

  private:
    const int _driveSpeed = 150;

    Truck* _truck;
    Gyro* _gyro;
    long _deviation = 0;

    bool _isRunning = false;
};

void ProgGoStraight::init(Truck &truck, Gyro &gyro) {
  _truck = &truck;
  _gyro = &gyro;
}

void ProgGoStraight::start() {
  if (!_isRunning) {
    _deviation = 0;
    _isRunning = true;
  }
}

void ProgGoStraight::stop() {
  _truck->stop();
  _isRunning = false;
}

void ProgGoStraight::tick() {
  if (!_isRunning) {
    return;
  }

  int c = _gyro->getCourse();
  _deviation += c;

  /*Serial.print(_deviation);
  Serial.print("\t");
  Serial.print(c);
  Serial.print("\t");
  Serial.print(200);
  Serial.print("\t");
  Serial.println(-200);
  return;*/

  int turn = -_deviation / 10000;
  _truck->goAndTurn(_driveSpeed, turn);

  /*if (_deviation > 0)
    _truck->goAndTurn(_driveSpeed, turn);
  else if (_deviation < 0)
    _truck->goAndTurn(_driveSpeed, -turn);
  else
    _truck->goAndTurn(_driveSpeed, 0);*/
}

bool ProgGoStraight::isRunning() {
  return _isRunning;
}
