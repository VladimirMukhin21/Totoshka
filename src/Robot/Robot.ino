#include "Radio.h"
#include "Truck.h"
#include "Hand.h"
#include "Tail.h"
#include "Video.h"
#include "ProgStairsUp.h"
#include "ProgStairsDown.h"
#include "ProgAutoLine.h"
#include "ProgTakeTin.h"
#include "ProgRotatePipe.h"
#include "Average.h"
#include "Color.h"
#include "Gyro.h"
#include "DistMeter.h"

#define L_EN_PIN 39
#define L_INA_PIN 41
#define L_INB_PIN 43
#define L_PWM_PIN 4  // ШИМ

#define R_EN_PIN 38
#define R_INA_PIN 40
#define R_INB_PIN 42
#define R_PWM_PIN 5  // ШИМ

// shoulder - elbow - rotate - claw
#define HAND_SHOULDER_PIN 11
#define HAND_ELBOW_PIN 9
#define HAND_ROTATE_PIN 8
#define HAND_CLAW_PIN 7

#define TAIL_COCCYX_PIN 10

#define CAMERA_SWITCHER_PIN 2
#define CAMERA_FRONT_PIN 12
#define CAMERA_TOP_PIN 6

#define LED_PIN 13

Radio radio;
Truck truck;
Hand hand;
Tail tail;
Video video;
Color color;
Gyro gyro;
DistMeter distMeter;
ProgStairsUp progStairsUp;
ProgStairsDown progStairsDown;
ProgAutoLine progAutoLine;
ProgTakeTin progTakeTin;
ProgRotatePipe progRotatePipe;

unsigned long lastRadioTime = millis();

unsigned long changeLedTime = millis();
bool ledStatus = true;

void setup() {
  Serial.begin(9600);

  // Эта строка устраняет зависание робота из-за гироскопа. Источник решения:
  // https://github.com/jrowberg/i2cdevlib/issues/519#issuecomment-752023021
  // Судя из описания метода setWireTimeout, это крайняя мера и лучше искать истинную проблему:
  // https://www.arduino.cc/reference/en/language/functions/communication/wire/setwiretimeout/
  // Другое решение говорит, что надо подключить пин AD0 гироскопа к GND:
  // https://www.i2cdevlib.com/forums/topic/414-freezing-problem/
  Wire.setWireTimeout(3000, true);

  radio.initReceiver();
  color.init();
  gyro.init();
  distMeter.init();
  truck.init(L_EN_PIN, L_INA_PIN, L_INB_PIN, L_PWM_PIN, R_EN_PIN, R_INA_PIN, R_INB_PIN, R_PWM_PIN, gyro);
  hand.init(HAND_SHOULDER_PIN, HAND_ELBOW_PIN, HAND_ROTATE_PIN, HAND_CLAW_PIN);
  tail.init(TAIL_COCCYX_PIN);
  video.init(CAMERA_SWITCHER_PIN, CAMERA_FRONT_PIN, CAMERA_TOP_PIN);

  progStairsUp.init(truck, tail);
  progStairsDown.init(truck, tail);
  progAutoLine.init(truck, hand, color);
  progTakeTin.init(truck, hand, distMeter);
  progRotatePipe.init(hand);

  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  if (!radio.available()) {
    if (millis() - lastRadioTime > 500) {
      stopAll();
      digitalWrite(LED_PIN, HIGH);
    }

    return;
  }

  //digitalWrite(LED_PIN, LOW);
  lastRadioTime = millis();

  Payload payload = radio.read();

  if (payload.frontRedButton) {
    stopAll();
    return;
  }

  tickAll();

  // мигаем светодиодом если не подвисли
  if (millis() - changeLedTime >= 500) {
    changeLedTime = millis();
    ledStatus = !ledStatus;
    digitalWrite(LED_PIN, ledStatus);
  }

  video.setActiveCamera(payload.frontBlackButtonSwitch);  // Переключение между камерами

  // если какая-то программа запущена, то роботом не управляем с пульта
  if (isAnyProgRunning()) {
    return;
  }

  if (payload.frontSwitch == 0) {
    if (payload.upGreenButton) {  // Рука на спину
      hand.handToBack();
    }
    else if (!payload.upBlueButton) {
      hand.operate(payload.leftStick.vert, payload.leftStick.horiz, 1);  // altMode
    }
  }
  else if (payload.frontSwitch == 1) {
    if (payload.upGreenButton) {  // Рука на спину
      hand.handToBack();
    }
    else if (!payload.upBlueButton) {
      hand.operate(payload.leftStick.vert, payload.leftStick.horiz, 0);  // НЕ altMode
    }
  }
  else if (payload.frontSwitch == 2) {
    if (payload.upGreenButton) {  // Хвост наверх
      tail.upTail();
    }
    else if (!payload.upBlueButton) {
      tail.operate(payload.leftStick.vert);
    }
  }

  if (payload.upBlueButton) {
    video.moveCamera(payload.leftStick.vert);  // Поворот камеры

    if (payload.frontYellowButton) {  // Захват маяка
      progTakeTin.start();
      return;
    }

    if (payload.key == 0xA1) {  // Езда прямо
      truck.goStraight(180);
      return;
    }
    else if (payload.key == 0xB1) {  // Езда прямо медленно
      truck.goStraight(60);
      return;
    }
    else if (payload.key == 0xC1) {  // Езда прямо задом
      truck.goStraight(-180);
      return;
    }
    else if (payload.key == 0xD1) {  // Езда прямо задом медленно
      truck.goStraight(-60);
      return;
    }
    else if (payload.key == 0xA2) {  // Рука на линию
      hand.handToRideTheLine();
    }
    else if (payload.key == 0xB2) {  // Авто-линия
      progAutoLine.start();
      return;
    }
    else if (payload.key == 0xC2) {  // Подъем по наклонной
      truck.goHill(200);             // едем быстро
      return;
    }
    else if (payload.key == 0xD2) {  // Спуск по наклонной
      truck.goHill(100);             // едем медленно
      return;
    }
    else if (payload.key == 0xA3) {  // Поворот трубки
      progRotatePipe.start();
      return;
    }
    else if (payload.key == 0xB3) {  // СВОБОДНО
    }
    else if (payload.key == 0xC3) {  // СВОБОДНО
    }
    else if (payload.key == 0xD3) {  // СВОБОДНО
    }
    else if (payload.key == 0xA4) {  // Подъем по лестнице
      progStairsUp.start();
      return;
    }
    else if (payload.key == 0xB4) {  // СВОБОДНО
    }
    else if (payload.key == 0xC4) {  // СВОБОДНО
    }
    else if (payload.frontWhiteButton && payload.key == 0xD4) {  // Калибровка гироскопа
      // хвостом показываем, что калибровка началась
      tail.moveTo(0);
      for (int i = 0; i < 100; i++) {
        tail.tick();
        delay(2);
      }

      delay(500);  // ждем, чтобы все успокоилось после движения хвоста
      gyro.calibrate();

      // хвостом показываем, что калибровка завершилась
      tail.moveTo(180);
      for (int i = 0; i < 100; i++) {
        tail.tick();
        delay(2);
      }

      return;
    }
  }

  truck.operate(payload.rightStick.vert, payload.rightStick.horiz);
}

bool isAnyProgRunning() {
  return progStairsUp.isRunning()
         || progStairsDown.isRunning()
         || progAutoLine.isRunning()
         || progTakeTin.isRunning()
         || progRotatePipe.isRunning();
}

void stopAll() {
  truck.stop(Motor::SMOOTH_OFF);
  hand.stop();
  tail.stop();
  video.stop();
  progStairsUp.stop();
  progStairsDown.stop();
  progAutoLine.stop();
  progTakeTin.stop();
  progRotatePipe.stop();
}

void tickAll() {
  truck.tick();
  hand.tick();
  tail.tick();
  video.tick();
  gyro.tick();
  progStairsUp.tick();
  progStairsDown.tick();
  progAutoLine.tick();
  progTakeTin.tick();
  progRotatePipe.tick();
}
