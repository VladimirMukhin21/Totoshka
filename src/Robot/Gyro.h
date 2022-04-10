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
    void tick();

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
  return _mpu.getRotationZ();
}

int Gyro::getRoll() {
  return _mpu.getRotationY();
}

int Gyro::getPitch() {
  return _mpu.getRotationX();
}

void Gyro::tick() {
  /*int ax, ay, az, gx, gy, gz, mx, my, mz;
  _mpu.getMotion9(&ax, &ay, &az, &gx, &gy, &gz, &mx, &my, &mz);
  Serial.print(gy);
  Serial.print("\t");
  Serial.print(_mpu.getRotationY());
  Serial.print("\t");
  //Serial.print(az);
  //Serial.print("\t");
  Serial.print(20000);
  Serial.print("\t");
  Serial.println(-20000);*/
}

/*byte Gyro::filterDeadZone(byte value) {
  if (value > -5 && value < 5)
    return 0;
  else
    return value;
  }*/
