#pragma once

#include <Wire.h>
#include <VL53L0X.h>

class DistMeter {
public:
  void init();
  void enable();
  void disable();

  int getDist();

private:
  VL53L0X sensor;
  unsigned long _lastReadTime = millis();
  int _lastValue = -2;
  bool _enabled = false;
};

void DistMeter::init() {
  Wire.begin();
  sensor.setTimeout(50);
  sensor.init();
  //enable();
  disable();
}

int DistMeter::getDist() {
  if (!_enabled) {
    return -1;
  }

  unsigned long now = millis();
  //if ((now - _lastReadTime >= 50) || (_lastValue < 0)) {
  if (now - _lastReadTime >= 50) {
    _lastReadTime = now;
    _lastValue = sensor.readRangeContinuousMillimeters();
  }

  return _lastValue;
}

void DistMeter::enable() {
  sensor.startContinuous();
  _enabled = true;
}

void DistMeter::disable() {
  //sensor.stopContinuous();
  //_lastValue = -2;
  _enabled = false;
}

// void DistMeter::tick() {
//   if (!_enabled) {
//     return;
//   }
//}