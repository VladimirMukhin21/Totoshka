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
};

void DistMeter::init() {
  Wire.begin();
  sensor.setTimeout(50);
  sensor.init();
  //enable();
  disable();
}

int DistMeter::getDist() {
  return sensor.readRangeContinuousMillimeters();
}

void DistMeter::enable() {
  sensor.startContinuous();
}

void DistMeter::disable() {
  sensor.stopContinuous();
}
