#include <Wire.h>
#include <VL53L0X.h>
#include <MPU6050.h>
#include "Average.h"

VL53L0X sensor;
MPU6050 mpu;

int16_t ax, ay, az;
int16_t gx, gy, gz;

Average dist;

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

  //delay(1000);
  Serial.print(dist.getAverage());
  Serial.print('\t');
  if (sensor.timeoutOccurred()) {
    Serial.print(" ТАЙМАУТ");

    sensor.setTimeout(50);
    while (!sensor.init()) {
      Serial.println("Не удалось обнаружить и инициализировать датчик!");
    }
  }


  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  Serial.print(ax); Serial.print('\t');
  Serial.print(ay); Serial.print('\t');
  Serial.print(az); Serial.print('\t');
  Serial.print(gx); Serial.print('\t');
  Serial.print(gy); Serial.print('\t');
  Serial.println(gz);
  delay(5);
}
