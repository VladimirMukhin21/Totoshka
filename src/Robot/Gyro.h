#pragma once

#include <Wire.h>
#include <MPU6050.h>
#include "Average.h"

class Gyro {
  public:
    void init();

    int getCourse();
    int getRoll();
    int getPitch();

  private:
    MPU6050 _mpu;

    /*int16_t accel_x, accel_y, accel_z;
      int16_t gyro_x, gyro_y, gyro_z;

      Average accelX;
      Average accelY;
      Average accelZ;
      Average gyroX;
      Average gyroY;
      Average gyroZ;

      byte filterDeadZone(byte value);*/
};

void Gyro::init() {
  Wire.begin();
  _mpu.initialize();
}

int Gyro::getCourse() {
  return _mpu.getRotationX();
}

int Gyro::getRoll() {
  return _mpu.getRotationY();
}

int Gyro::getPitch() {
  return _mpu.getRotationZ();
}

/*byte Gyro::filterDeadZone(byte value) {
  if (value > -5 && value < 5)
    return 0;
  else
    return value;
}*/
