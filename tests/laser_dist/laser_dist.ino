#include <Wire.h>
#include <VL53L0X.h>

VL53L0X sensor;

// Раскомментируйте строку ниже, чтобы использовать дальний режим.
// Это увеличивает чувствительность датчика и расширяет его потенциальный диапазон,
// но увеличивает вероятность получения неточных показаний
// из-за отражений от объектов, отличных от намеченной цели.
// Лучше всего работает в темноте.

//#define LONG_RANGE

// Раскомментируйте одну из двух строк ниже, чтобы получить:
// - более высокую скорость за счет меньшей точности ИЛИ
// - более высокую точность за счет меньшей скорости

//#define HIGH_SPEED
//#define HIGH_ACCURACY

int dist = 0;

void setup()
{
  Serial.begin(9600);
  Wire.begin();

  sensor.setTimeout(50);
  if (!sensor.init())
  {
    Serial.println("Не удалось обнаружить и инициализировать датчик!");
    while (1) {}
  }

#if defined LONG_RANGE
  // понижает предел скорости обратного сигнала (по умолчанию 0,25 MCPS (мчип/с))
  sensor.setSignalRateLimit(0.1);
  // увеличить периоды лазерного импульса (по умолчанию 14 и 10 PCLK)
  // * - PCLK — это частота периферии
  sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
  sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
#endif

#if defined HIGH_SPEED
  // уменьшить тайминг до 20 мс (по умолчанию около 33 мс)
  sensor.setMeasurementTimingBudget(20000);
#elif defined HIGH_ACCURACY
  // увеличить тайминг 200 мс
  sensor.setMeasurementTimingBudget(200000);
#endif
}

void loop()
{
  for (int i = 0; i <= 10; i++)
  {
    dist += sensor.readRangeSingleMillimeters() - 30;
  }
  dist = dist / 10;


  //delay(1000);
  Serial.print(dist);
  if (sensor.timeoutOccurred()) {
    Serial.print(" ТАЙМАУТ");

    sensor.setTimeout(50);
    while (!sensor.init()) {
      Serial.println("Не удалось обнаружить и инициализировать датчик!");
    }
  }
  Serial.println();
}
