#pragma once

#include <Wire.h>
#include <MPU6050.h>
#include "Average.h"

MPU6050 mpu;

int16_t angle_x, angle_y, angle_z;
int16_t gravity_x, gravity_y, gravity_z;

Average angleX;
Average angleY;
Average angleZ;
Average gravityX;
Average gravityY;
Average gravityZ;

class Gyro {
  public:
    void init();

  private:
    byte filterDeadZone(byte value);
};

void Gyro::init() {
  Wire.begin();
  mpu.initialize();
}

byte Gyro::filterDeadZone(byte value) {
  if (value > -5 && value < 5)
    return 0;
  else
    return value;
}
