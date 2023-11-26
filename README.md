# Тотошка

## Установка библиотек

В папке libraries находятся используемые библиотеки (для установки скопировать сюда: `%USERPROFILE%\Documents\Arduino\libraries`).

Используемые библиотеки:
* `\libraries\RF24-master` - работа с NRF.
* `\libraries\i2cdevlib\Arduino\MPU6050` + `\libraries\i2cdevlib\Arduino\I2Cdev` - гироскоп.
  [Источник](https://github.com/jrowberg/i2cdevlib).
* `\libraries\VL53L0X` - лазерный датчик расстояния.
  [Источник](https://github.com/pololu/vl53l0x-arduino).
* `\libraries\Adafruit_TCS34725_SoftI2C` - датчик цвета.
  [Источник](https://github.com/Fire7/Adafruit_TCS34725_SoftI2C).
  Вручную добавлена поддержка нашего типа датчика 0x4D - файл Adafruit_TCS34725softi2c.cpp, метод begin, стр. 157: `if ((x != 0x44) && (x != 0x4D))`.
* `\libraries\SoftwareWire` - программный i2c. Нужен для работы с двумя датчиками цвета, т.к. в них нельзя изменить адрес на шине i2c.
  [Источник](https://github.com/Fire7/SoftwareWire).
* `\libraries\EncButton` - библиотека для энкодера, энкодера с кнопкой или просто кнопки с Arduino.
  [Источник](https://github.com/GyverLibs/EncButton).
* `\libraries\GyverPID` - PID регулятор.
  [Источник](https://github.com/GyverLibs/GyverPID).
* `\libraries\Keypad` - работа с матричной клавиатурой.
  [Источник](https://github.com/Chris--A/Keypad).


## Управление пультом

* Тележка - правый джойстик
* Рука
  * Вверх/вниз параллельно - тумблер центр (1) + левый джойстик вверх/вниз
  * Сжать/разжать - тумблер центр (1) + левый джойстик влево/вправо
  * Локоть - тумблер вверх (0) + левый джойстик вверх/вниз
  * Поворот запястья - тумблер вверх (0) + левый джойстик влево/вправо
* Хвост - тумблер вниз (2) + левый джойстик вверх/вниз
* Стоп всего - красная кнопка
* Камера
  * Переключение между камерами - черная кнопка
  * Поворот камеры - синяя + лев джойстик вверх/вниз
* Автоматические действия
  * Рука на спину - тумблер вверх/центр (0/1) + зеленая кнопка
  * Хвост наверх - тумблер вниз (2) + зеленая кнопка
  * Поворот трубки - синяя + A3
  * Рука на линию - синяя + A2
  * Поворот:
    * налево на 90 град - синяя + B4
    * направо на 90 град - синяя + C4
* Автономные программы
  * Захват маяка - синяя + желтая кнопка
  * Езда прямо:
    * быстро - синяя + A1
    * медленно - синяя + B1
    * задом быстро - синяя + C1
    * задом медленно - синяя + D1
  * Авто-линия - синяя + B2
  * Наклонная:
    * подъем - синяя + C2
    * спуск - синяя + D2
  * Подъем по лестнице - синяя + A4
  * Горка с трубами:
    * налево - синяя + B3
    * прямо - синяя + C3
    * направо - синяя + D3
  * Провалы:
    * налево - белая + B3
    * прямо - белая + C3
    * направо - белая + D3
  * Копыто:
    * налево - белая + A4
    * направо - белая + B4
  * Калибровка гироскопа - синяя + белая + D4
