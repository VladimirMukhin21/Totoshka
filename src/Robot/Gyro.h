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
  //_mpu.CalibrateAccel(6);
  //_mpu.CalibrateGyro(6);
}

// курс
int Gyro::getCourse() {
  return _mpu.getRotationZ();
}

// крен
int Gyro::getRoll() {
  return _mpu.getRotationY();
}

// тангаж
int Gyro::getPitch() {
  return _mpu.getRotationX();
}

void Gyro::tick() {
  //int ax, ay, az, gx, gy, gz;
  //_mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  /*ax = _mpu.getAccelerationX();
  ay = _mpu.getAccelerationY();
  az = _mpu.getAccelerationZ();
  gx = _mpu.getRotationX();
  gy = _mpu.getRotationY();
  gz = _mpu.getRotationZ();*/

  /*//Serial.print(az);
  //Serial.print("\t");
  Serial.print(gz);
  //Serial.print("\t0\t20000\t-20000"); // линии для масштаба на графике
  Serial.println();*/
}

/*byte Gyro::filterDeadZone(byte value) {
  if (value > -5 && value < 5)
    return 0;
  else
    return value;
  }*/
