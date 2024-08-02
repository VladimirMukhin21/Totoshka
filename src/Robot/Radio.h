#pragma once

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "DistMeter.h"
#include "Color.h"

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
  bool switchTelemetry;
};

struct Telemetry {
  int dist;
  byte clrl;
  byte clrr;
};

class Radio {
public:
  Radio();
  void initReceiver(DistMeter &distMeter, Color &color);
  bool available();
  Payload read();

private:
  const byte _channel = 0x60;
  const byte _pipeNo = 1;
  const byte _payloadSize = 32;

  unsigned long _lastReloadDataTime = millis();

  bool _isTelemetryOn = false;

  RF24 _nrf;
  DistMeter *_distMeter;
  Color *_color;

  void initCommon();
};

Radio::Radio()
  : _nrf(48, 49) {
}

void Radio::initReceiver(DistMeter &distMeter, Color &color) {
  initCommon();

  _distMeter = &distMeter;
  _color = &color;

  byte address[][6] = { "1Toto", "2Toto", "3Toto", "4Toto", "5Toto", "6Toto" };  // возможные номера труб
  _nrf.openReadingPipe(_pipeNo, address[0]);                                     // выбираем трубу
  _nrf.startListening();                                                         // начинаем слушать эфир, мы приёмный модуль
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

  if (payload.switchTelemetry == true) {
    if (!_isTelemetryOn) {
      _distMeter->enable();
      _color->enable();
      Serial.println("On");
      _isTelemetryOn = true;
    }

    if (millis() - _lastReloadDataTime >= 500) {
      _lastReloadDataTime = millis();

      Telemetry telemetry;
      telemetry.dist = _distMeter->getDist();
      telemetry.clrl = _color->getLeft();
      telemetry.clrr = _color->getRight();
      /*Serial.print(telemetry.clrl);
      Serial.print("\t");
      Serial.println(telemetry.clrr);*/

      if (telemetry.dist == -1) {
        _distMeter->disable();
        _distMeter->enable();
      }

      _nrf.writeAckPayload(_pipeNo, &telemetry, sizeof(telemetry));
    }
  }
  else {
    if (_isTelemetryOn) {
      _distMeter->disable();
      _color->disable();
      Serial.println("Off");
      _isTelemetryOn = false;
    }
  }
  return payload;
}
