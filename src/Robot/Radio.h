#pragma once

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "DistMeter.h"

struct Stick {
  byte vert;
  byte horiz;
  bool pressed;  //: 1;
};

struct Payload {
  Stick leftStick;
  Stick rightStick;

  byte frontSwitch;  //: 2;

  bool frontRedButton;          //: 1;
  bool frontYellowButton;       //: 1;
  bool frontWhiteButton;        //: 1;
  byte frontBlackButtonSwitch;  //: 2;

  bool upGreenButton;  //: 1;
  bool upBlueButton;   //: 1;
  byte key;
};

struct Telemetry {
  int dist;   //: 2;
  int data2;  //: 2;
};

class Radio {
public:
  Radio();
  void initReceiver(DistMeter &distMeter);
  void initTransmitter();
  bool available();
  Payload read();
  void write(Payload payload);

private:
  const byte _channel = 0x60;
  const byte _pipeNo = 1;
  const byte _payloadSize = 32;

  unsigned long _lastReloadDataTime = millis();

  RF24 _nrf;
  DistMeter *_distMeter;

//#define DEBUG
#ifdef DEBUG
  unsigned long _printTime = millis();
#endif

  void initCommon();
  void debugPrint(Payload payload);
};

Radio::Radio()
  : _nrf(48, 49) {
}

void Radio::initReceiver(DistMeter &distMeter) {
  initCommon();

  _distMeter = &distMeter;

  byte address[][6] = { "1Toto", "2Toto", "3Toto", "4Toto", "5Toto", "6Toto" };  // возможные номера труб
  _nrf.openReadingPipe(_pipeNo, address[0]);                                     // выбираем трубу
  _nrf.startListening();                                                         // начинаем слушать эфир, мы приёмный модуль
}

void Radio::initTransmitter() {
  initCommon();

  byte address[][6] = { "1Toto", "2Toto", "3Toto", "4Toto", "5Toto", "6Toto" };  // возможные номера труб
  _nrf.openWritingPipe(address[0]);                                              // !!! мы - труба 0, открываем канал для передачи данных
  _nrf.stopListening();                                                          // не слушаем радиоэфир, мы передатчик
}

void Radio::initCommon() {
  _nrf.begin();                       // активировать модуль
  _nrf.setAutoAck(1);                 // режим подтверждения приёма, 1 вкл 0 выкл
  _nrf.setRetries(0, 15);             // (время между попыткой достучаться, число попыток)
  _nrf.enableAckPayload();            // разрешить отсылку данных в ответ на входящий сигнал
  _nrf.setPayloadSize(_payloadSize);  // размер пакета, байт
  _nrf.setChannel(_channel);          // выбираем канал (в котором нет шумов!)

  // Мощность: RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
  _nrf.setPALevel(RF24_PA_LOW);  // уровень мощности передатчика

  // Скорость: RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
  // ВНИМАНИЕ!!! enableAckPayload НЕ РАБОТАЕТ НА СКОРОСТИ 250 kbps!
  _nrf.setDataRate(RF24_1MBPS);  // скорость обмена (ниже скорость - выше чувствительность и дальность)

  _nrf.powerUp();  // начать работу
}

bool Radio::available() {
  byte pipeNo;
  return _nrf.available(&pipeNo);
}

Payload Radio::read() {
  Payload payload;
  _nrf.read(&payload, sizeof(payload));

  if (millis() - _lastReloadDataTime >= 500) {
    _lastReloadDataTime = millis();

    Telemetry telemetry;
    //_distMeter->enable();
    telemetry.dist = _distMeter->getDist();
    //_distMeter->disable();
    telemetry.data2 = 5678;

    _nrf.writeAckPayload(_pipeNo, &telemetry, sizeof(telemetry));
  }

#ifdef DEBUG
  debugPrint(payload);
#endif
  return payload;
}

void Radio::write(Payload payload) {
  _nrf.write(&payload, sizeof(payload));

  if (_nrf.available()) {
    Telemetry telemetry;
    _nrf.read(&telemetry, sizeof(telemetry));
    Serial.print(telemetry.dist);
    Serial.print("\t");
    Serial.println(telemetry.data2);
  }

#ifdef DEBUG
  debugPrint(payload);
#endif
}

#ifdef DEBUG
void Radio::debugPrint(Payload payload) {
  if (millis() - _printTime >= 10) {
    _printTime = millis();

    /*Serial.print("\tfrontSwitch = ");
    Serial.print(payload.frontSwitch);

    Serial.print("\tredB = ");
    Serial.print(payload.frontRedButton);
    Serial.print("\tyellowB = ");
    Serial.print(payload.frontYellowButton);
    Serial.print("\twhiteB = ");
    Serial.print(payload.frontWhiteButton);
    Serial.print("\tblackB = ");
    Serial.print(payload.frontBlackButtonSwitch);

    Serial.println();*/
  }
}
#endif
