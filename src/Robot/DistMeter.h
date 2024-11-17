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
  int _lastValue = 0;
};

void DistMeter::init() {
  Wire.begin();
  sensor.setTimeout(50);
  sensor.init();
  //enable();
  disable();
}

int DistMeter::getDist() {
  unsigned long now = millis();
  if (now - _lastReadTime >= 50) {
    _lastReadTime = now;
    _lastValue = sensor.readRangeContinuousMillimeters();
  }

  return _lastValue;
}

void DistMeter::enable() {
  sensor.startContinuous();
}

void DistMeter::disable() {
  sensor.stopContinuous();
}
