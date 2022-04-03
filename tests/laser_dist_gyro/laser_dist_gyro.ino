#include <Wire.h>
#include <VL53L0X.h>
VL53L0X sensor;

#include <MPU6050.h>
MPU6050 mpu;

int16_t ax, ay, az;
int16_t gx, gy, gz;

int dist = 0;

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
for (int i = 0; i <= 10; i++)
  {
    dist += sensor.readRangeSingleMillimeters() - 30;
  }
  dist = dist / 10;

  //delay(1000);
  Serial.print(dist);
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
