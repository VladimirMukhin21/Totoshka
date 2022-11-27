# Тотошка

## Установка библиотек

В папке libraries находятся используемые библиотеки. Для их установки их надо скопировать по одному из следующих путей:
* 64-битная версия Windows – `C:\Program Files (x86)\Arduino\libraries\`;
* 32-битная версия Windows – `C:\Program Files\Arduino\libraries\`.

Используемые библиотеки:
* `\libraries\RF24-master` - работа с NRF.
* `\libraries\i2cdevlib\Arduino\MPU6050` + `\libraries\i2cdevlib\Arduino\I2Cdev` - гироскоп.
* `\libraries\VL53L0X` - лазерный датчик расстояния.
* `\libraries\Adafruit_TCS34725_SoftI2C` - датчик цвета. [Источник](https://github.com/Fire7/Adafruit_TCS34725_SoftI2C), добавлена поддержка нашего типа датчика (метод begin, датчик 0x4D).
* `\libraries\SoftwareWire` - программный i2c. Нужен для работы с двумя датчиками цвета, т.к. в них нельзя изменить адрес на шине i2c. [Источник](https://github.com/Fire7/SoftwareWire).
* `\libraries\EncButton` - лёгкая и функциональная библиотека для энкодера, энкодера с кнопкой или просто кнопки с Arduino ([источник](https://github.com/GyverLibs/EncButton)).
* `\libraries\GyverPID` - PID регулятор ([источник](https://github.com/GyverLibs/GyverPID)).
