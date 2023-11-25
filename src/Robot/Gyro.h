#pragma once

#include <Wire.h>
#include <MPU6050.h>
#include <EEPROM.h>
#include "Average.h"

class Gyro {
public:
  void init();
  void enable();
  void disable();
  int getDeltaCourse();
  int getDeltaRoll();
  int getDeltaPitch();
  int getRoll();
  int getPitch();
  void tick();
  void calibrate();

private:
  static const int DEFAULT_AUTO_DISABLE_TIME = 3000;
  static const byte OFFSETS_ADDRESS = 0;

  MPU6050 _mpu;
  Average _avgAX, _avgAY, _avgAZ, _avgGX, _avgGY, _avgGZ;
  bool _enabled = false;
  int _autoDisableTime = DEFAULT_AUTO_DISABLE_TIME;  // если гироскоп не используется, то через это время он выключается
  unsigned long _lastGetTime = millis();             // время последнего вызова одного из методов get
  // unsigned long _tickTime = millis();

  void onBeforeGet();
  void setAutoDisableTime(int msec = DEFAULT_AUTO_DISABLE_TIME);

  void saveOffsets();
  void loadOffsets();
  void printOffsets();
};

void Gyro::init() {
  Wire.begin();
  _mpu.initialize();
  loadOffsets();
}

void Gyro::enable() {
  if (_enabled) {
    return;
  }

  _enabled = true;
  tick();  // сразу читаем показания, чтобы их можно было получить методами get
}

void Gyro::disable() {
  if (!_enabled) {
    return;
  }

  _enabled = false;
  _avgAX.reset();
  _avgAY.reset();
  _avgAZ.reset();
  _avgGX.reset();
  _avgGY.reset();
  _avgGZ.reset();
}

// изменение курса
int Gyro::getDeltaCourse() {
  onBeforeGet();
  return _avgGZ.getAverage();
  // return _mpu.getRotationZ();
}

// изменение крена
int Gyro::getDeltaRoll() {
  onBeforeGet();
  return _avgGY.getAverage();
  // return _mpu.getRotationY();
}

// изменение тангажа
int Gyro::getDeltaPitch() {
  onBeforeGet();
  return _avgGX.getAverage();
  // return _mpu.getRotationX();
}

// крен
int Gyro::getRoll() {
  onBeforeGet();
  return _avgAX.getAverage();
  // return _mpu.getAccelerationX();
}

// тангаж
int Gyro::getPitch() {
  onBeforeGet();
  return _avgAY.getAverage();
  // return _mpu.getAccelerationY();
}

void Gyro::tick() {
  if (!_enabled) {
    return;
  }

  if ((_autoDisableTime >= 0) && (millis() - _lastGetTime > _autoDisableTime)) {
    disable();  // гироскоп долго не используется => выключаем его
    return;
  }

  // if (millis() - _tickTime < 1) {
  //   return;
  // }
  // _tickTime = millis();

  int ax, ay, az, gx, gy, gz;
  _mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  _avgAX.add(ax);
  _avgAY.add(ay);
  _avgAZ.add(az);
  _avgGX.add(gx);
  _avgGY.add(gy);
  _avgGZ.add(gz);

  // Serial.print(_avgGX.getAverage());
  // Serial.print("\t");
  // Serial.print(_avgGY.getAverage());
  // Serial.print("\t");
  // Serial.print(_avgGZ.getAverage());
  // Serial.print("\t");

  // Serial.print(ax);
  // Serial.print("\t");
  // Serial.print(ay);
  // Serial.print("\t");
  // Serial.print(az);
  // Serial.print("\t");

  // Serial.print("0\t50\t-50");  // линии для масштаба на графике
  // Serial.println();
}

void Gyro::calibrate() {
  // _mpu.CalibrateAccel(10);
  // _mpu.CalibrateGyro(10);
  // return;

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

  saveOffsets();
}

void Gyro::onBeforeGet() {
  enable();
  _lastGetTime = millis();
}

void Gyro::setAutoDisableTime(int msec = DEFAULT_AUTO_DISABLE_TIME) {
  _autoDisableTime = msec;
}

void Gyro::saveOffsets() {
  int offsets[6];
  offsets[0] = _mpu.getXAccelOffset();
  offsets[1] = _mpu.getYAccelOffset();
  offsets[2] = _mpu.getZAccelOffset();
  offsets[3] = _mpu.getXGyroOffset();
  offsets[4] = _mpu.getYGyroOffset();
  offsets[5] = _mpu.getZGyroOffset();

  EEPROM.put(OFFSETS_ADDRESS, offsets);

  Serial.print("Gyro offsets (saved): ");
  printOffsets();
}

void Gyro::loadOffsets() {
  int offsets[6];
  EEPROM.get(OFFSETS_ADDRESS, offsets);

  _mpu.setXAccelOffset(offsets[0]);
  _mpu.setYAccelOffset(offsets[1]);
  _mpu.setZAccelOffset(offsets[2]);
  _mpu.setXGyroOffset(offsets[3]);
  _mpu.setYGyroOffset(offsets[4]);
  _mpu.setZGyroOffset(offsets[5]);

  Serial.print("Gyro offsets (loaded): ");
  printOffsets();
}

void Gyro::printOffsets() {
  Serial.print("ax = ");
  Serial.print(_mpu.getXAccelOffset());

  Serial.print("; ay = ");
  Serial.print(_mpu.getYAccelOffset());

  Serial.print("; az = ");
  Serial.print(_mpu.getZAccelOffset());

  Serial.print("; gx = ");
  Serial.print(_mpu.getXGyroOffset());

  Serial.print("; gy = ");
  Serial.print(_mpu.getYGyroOffset());

  Serial.print("; gz = ");
  Serial.print(_mpu.getZGyroOffset());

  Serial.println();
}
