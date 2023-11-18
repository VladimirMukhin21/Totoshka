#include "Radio.h"
#include "Truck.h"
#include "Hand.h"
#include "Tail.h"
#include "Video.h"
#include "ProgStairsUp.h"
#include "ProgStairsDown.h"
#include "ProgGoStraight.h"
#include "ProgRideTheLine.h"
#include "ProgTakeTin.h"
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
ProgGoStraight progGoStraight;
ProgRideTheLine progRideTheLine;
ProgTakeTin progTakeTin;

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
  truck.init(L_EN_PIN, L_INA_PIN, L_INB_PIN, L_PWM_PIN, R_EN_PIN, R_INA_PIN, R_INB_PIN, R_PWM_PIN);
  hand.init(HAND_SHOULDER_PIN, HAND_ELBOW_PIN, HAND_ROTATE_PIN, HAND_CLAW_PIN);
  tail.init(TAIL_COCCYX_PIN);
  video.init(CAMERA_SWITCHER_PIN, CAMERA_FRONT_PIN, CAMERA_TOP_PIN);
  color.init();
  gyro.init();
  distMeter.init();

  progStairsUp.init(truck, tail);
  progStairsDown.init(truck, tail);
  progGoStraight.init(truck, gyro);
  progRideTheLine.init(truck, hand, color);
  progTakeTin.init(truck, hand, distMeter);

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

  video.setActiveCamera(payload.frontBlackButtonSwitch);

  // если какая-то программа запущена, то роботом не управляем с пульта
  if (isAnyProgRunning()) {
    return;
  }

  if (payload.frontSwitch == 0) {
    if (payload.upGreenButton) {
      hand.handToBack();
    }
    else if (!payload.upBlueButton) {
      hand.operate(payload.leftStick.vert, payload.leftStick.horiz, 1); // altMode
    }
  }
  else if (payload.frontSwitch == 1) {
    if (payload.upGreenButton) {
      hand.handToBack();
    }
    else if (payload.frontYellowButton && payload.upBlueButton) {
      progTakeTin.start();
      return;
    }
    else if (payload.frontWhiteButton && payload.upBlueButton) {
      progGoStraight.start();
      return;
    }
    else if (!payload.upBlueButton) {
      hand.operate(payload.leftStick.vert, payload.leftStick.horiz, 0); // НЕ altMode
    }
  }
  else if (payload.frontSwitch == 2) {
    if (payload.upGreenButton && !payload.upBlueButton) {
      tail.upTail();
    }
    else if (payload.frontYellowButton && payload.upBlueButton) {
      progStairsUp.start();
      return;
    }
    else if (payload.frontWhiteButton && payload.upBlueButton) {
      progRideTheLine.start();
      return;
    }
    else if (payload.upGreenButton && payload.upBlueButton) {
      hand.handToRideTheLine();
    }
    else if (!payload.upBlueButton) {
      tail.operate(payload.leftStick.vert);
    }
  }

  if (payload.upBlueButton) {
    video.moveCamera(payload.leftStick.vert);
  }

  truck.go(payload.rightStick.vert, payload.rightStick.horiz);
}

bool isAnyProgRunning() {
  return progStairsUp.isRunning()
         || progStairsDown.isRunning()
         || progRideTheLine.isRunning()
         || progGoStraight.isRunning()
         || progTakeTin.isRunning();
}

void stopAll() {
  truck.stop();
  hand.stop();
  tail.stop();
  video.stop();
  progStairsUp.stop();
  progStairsDown.stop();
  progRideTheLine.stop();
  progGoStraight.stop();
  progTakeTin.stop();
}

void tickAll() {
  truck.tick();
  hand.tick();
  tail.tick();
  video.tick();
  gyro.tick();
  progStairsUp.tick();
  progStairsDown.tick();
  progRideTheLine.tick();
  progGoStraight.tick();
  progTakeTin.tick();
}
