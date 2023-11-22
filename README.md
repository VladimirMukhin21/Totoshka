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
* `\libraries\EncButton` - лёгкая и функциональная библиотека для энкодера, энкодера с кнопкой или просто кнопки с Arduino.
  [Источник](https://github.com/GyverLibs/EncButton).
* `\libraries\GyverPID` - PID регулятор.
  [Источник](https://github.com/GyverLibs/GyverPID).


## Управление пультом

* Правый джойстик - управление тележкой.
* Красная кнопка - стоп всего.
* Черная - переключение текущей камеры.
* Синяя + лев джойстик вверх/вниз - управление текущей камерой.
* Тумблер вверх (0):
  * зеленая кнопка - рука на спину
  * желтая + синяя кнопка кнопка - поворот трубок
  * белая + синяя  кнопка - подъем/спуск по наклонной (ВРЕМЕННО ЗДЕСЬ)
  * левый джойстик:
    * вверх/вниз - локоть
	* влево/вправо - поворот запястья
* Тумблер центр (1):
  * зеленая кнопка - рука на спину
  * зеленая + синяя кнопка - опускание руки для езды по линии
  * желтая + синяя кнопка - захват маяка
  * белая + синяя кнопка - езда прямо
  * левый джойстик:
    * вверх/вниз - подъем/опускание параллельно;
	* влево/вправо - сжатие/разжатие;
* Тумблер вниз (2):
  * зеленая кнопка - хвост вверх
  * желтая + синяя кнопка - подъем по лестнице
  * белая + синяя кнопка - езда по линии
  * левый джойстик:
    * вверх/вниз - управление хвостом
