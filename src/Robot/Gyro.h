#pragma once

#include <Wire.h>
#include <MPU6050.h>
#include "Average.h"

class Gyro {
public:
  void init();

  int getDeltaCourse();
  int getDeltaRoll();
  int getDeltaPitch();
  int getRoll();
  int getPitch();
  void tick();

private:
  MPU6050 _mpu;

  /*int16_t accel_x, accel_y, accel_z;
      int16_t gyro_x, gyro_y, gyro_z;*/

  // Average avgAX;
  // Average avgAY;
  // Average avgAZ;
  // Average avgGX;
  // Average avgGY;
  // Average avgGZ;

  void calibrate();
  //byte filterDeadZone(byte value);
};

void Gyro::init() {
  Wire.begin();
  _mpu.initialize();
  calibrate();
  // _mpu.CalibrateAccel(10);
  // _mpu.CalibrateGyro(10);
}

// изменение курса
int Gyro::getDeltaCourse() {
  return _mpu.getRotationZ();
}

// изменение крена
int Gyro::getDeltaRoll() {
  return _mpu.getRotationY();
}

// изменение тангажа
int Gyro::getDeltaPitch() {
  return _mpu.getRotationX();
}

// крен
int Gyro::getRoll() {
  return _mpu.getAccelerationX();
}

// тангаж
int Gyro::getPitch() {
  return _mpu.getAccelerationY();
}

void Gyro::tick() {
  // Serial.print(getPitch());
  // Serial.print("\t");

  // int ax, ay, az, gx, gy, gz;
  // _mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // avgGX.add(gx);
  // avgGY.add(gy);
  // avgGZ.add(gz);

  //gz = (int)(gz / 32768 * 250);

  // Serial.print(avgGX.getAverage());
  // Serial.print("\t");
  // Serial.print(avgGY.getAverage());
  // Serial.print("\t");
  // Serial.print(avgGZ.getAverage());
  // Serial.print("\t");

  // Serial.print(ax);
  // Serial.print("\t");
  // Serial.print(ay);
  // Serial.print("\t");
  // Serial.print(az);
  // Serial.print("\t");

  // Serial.print(gx);
  // Serial.print("\t");
  // Serial.print(gy);
  // Serial.print("\t");
  // Serial.print(gz);
  // Serial.print("\t");

  // Serial.print("0\t50\t-50");  // линии для масштаба на графике
  // Serial.println();
}

void Gyro::calibrate() {
  long offsets[6];
  long offsetsOld[6];
  int16_t mpuGet[6];
  const int BUFFER_SIZE = 100;

  // используем стандартную точность
  _mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
  _mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_250);

  // обнуляем оффсеты
  _mpu.setXAccelOffset(0);
  _mpu.setYAccelOffset(0);
  _mpu.setZAccelOffset(0);
  _mpu.setXGyroOffset(0);
  _mpu.setYGyroOffset(0);
  _mpu.setZGyroOffset(0);
  delay(10);

  for (byte n = 0; n < 10; n++) {   // 10 итераций калибровки
    for (byte j = 0; j < 6; j++) {  // обнуляем калибровочный массив
      offsets[j] = 0;
    }

    for (byte i = 0; i < 100 + BUFFER_SIZE; i++) {
      // делаем BUFFER_SIZE измерений для усреднения
      _mpu.getMotion6(&mpuGet[0], &mpuGet[1], &mpuGet[2], &mpuGet[3], &mpuGet[4], &mpuGet[5]);

      // пропускаем первые 99 измерений
      if (i >= 99) {
        for (byte j = 0; j < 6; j++) {
          offsets[j] += (long)mpuGet[j];  // записываем в калибровочный массив
        }
      }
    }

    for (byte i = 0; i < 6; i++) {
      offsets[i] = offsetsOld[i] - ((long)offsets[i] / BUFFER_SIZE);  // учитываем предыдущую калибровку
      if (i == 2) offsets[i] += 16384;                                // если ось Z, калибруем в 16384
      offsetsOld[i] = offsets[i];
    }

    // ставим новые оффсеты
    _mpu.setXAccelOffset(offsets[0] / 8);
    _mpu.setYAccelOffset(offsets[1] / 8);
    _mpu.setZAccelOffset(offsets[2] / 8);
    _mpu.setXGyroOffset(offsets[3] / 4);
    _mpu.setYGyroOffset(offsets[4] / 4);
    _mpu.setZGyroOffset(offsets[5] / 4);
    delay(2);
  }
}

/*byte Gyro::filterDeadZone(byte value) {
  if (value > -5 && value < 5)
    return 0;
  else
    return value;
  }*/
