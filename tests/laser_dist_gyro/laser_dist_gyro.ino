#include <Wire.h>
#include <VL53L0X.h>
#include <MPU6050.h>
#include "Average.h"

VL53L0X sensor;
MPU6050 mpu;

int16_t angle_x, angle_y, angle_z;
int16_t gravity_x, gravity_y, gravity_z;

Average dist;
Average angleX;
Average angleY;
Average angleZ;
Average gravityX;
Average gravityY;
Average gravityZ;


void setup() {
  Wire.begin();
  Serial.begin(9600);

  sensor.setTimeout(50);
  if (!sensor.init())
  {
    Serial.println("Не удалось обнаружить и инициализировать датчик!");
    while (1) {}
  }

  mpu.initialize();
  // состояние соединения
  Serial.println(mpu.testConnection() ? "MPU6050 OK" : "MPU6050 FAIL");
  delay(1000);
}

void loop() {
  dist.add(sensor.readRangeSingleMillimeters() - 30);

  Serial.print(dist.getAverage());
  Serial.print('\t');
  if (sensor.timeoutOccurred()) {
    Serial.print(" ТАЙМАУТ");

    sensor.setTimeout(50);
    while (!sensor.init()) {
      Serial.println("Не удалось обнаружить и инициализировать датчик!");
    }
  }

  mpu.getMotion6(&angle_x, &angle_y, &angle_z, &gravity_x, &gravity_y, &gravity_z);
  angleX.add(angle_x);
  angleY.add(angle_y);
  angleZ.add(angle_z);
  gravityX.add(gravity_x);
  gravityY.add(gravity_y);
  gravityZ.add(gravity_z);

  Serial.print(angleX.getAverage()); Serial.print('\t');
  Serial.print(angleY.getAverage()); Serial.print('\t');
  Serial.print(angleZ.getAverage()); Serial.print('\t');
  Serial.print(gravityX.getAverage()); Serial.print('\t');
  Serial.print(gravityY.getAverage()); Serial.print('\t');
  Serial.print(gravityZ.getAverage());
  Serial.println();
  delay(5);
}
