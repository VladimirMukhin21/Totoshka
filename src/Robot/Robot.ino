#include "Radio.h"
#include "Truck.h"
#include "Hand.h"
#include "Tail.h"
#include "Video.h"
#include "ProgStairsUp.h"
#include "ProgStairsDown.h"
#include "ProgGoStraight.h"
#include "ProgRideTheLine.h"
#include "Average.h"
#include "Color.h"
//#include "Gyro.h"

#define L_EN_PIN  39
#define L_INA_PIN 41
#define L_INB_PIN 43
#define L_PWM_PIN 4 // ШИМ

#define R_EN_PIN  38
#define R_INA_PIN 40
#define R_INB_PIN 42
#define R_PWM_PIN 5 // ШИМ

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
//Gyro gyro;
ProgStairsUp progStairsUp;
ProgStairsDown progStairsDown;
//ProgGoStraight progGoStraight;
ProgRideTheLine progRideTheLine;

unsigned long lastRadioTime = millis();

unsigned long changeLedTime = millis();
bool ledStatus = true;

void setup() {
  Serial.begin(9600);
  radio.initReceiver();
  truck.init(L_EN_PIN, L_INA_PIN, L_INB_PIN, L_PWM_PIN, R_EN_PIN, R_INA_PIN, R_INB_PIN, R_PWM_PIN);
  hand.init(HAND_SHOULDER_PIN, HAND_ELBOW_PIN, HAND_ROTATE_PIN, HAND_CLAW_PIN);
  tail.init(TAIL_COCCYX_PIN);
  video.init(CAMERA_SWITCHER_PIN, CAMERA_FRONT_PIN, CAMERA_TOP_PIN);
  color.init();
  //gyro.init();

  progStairsUp.init(truck, tail);
  progStairsDown.init(truck, tail);
  //progGoStraight.init(truck, gyro);
  progRideTheLine.init(truck, hand, color);

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

  if (payload.frontYellowButton) {
    progStairsUp.start();
  }
  else if (payload.frontWhiteButton) {
    //progStairsDown.start();
    progRideTheLine.start();
  }
  /*else if (payload.frontBlackButton) {
    progGoStraight.start();
  }*/
  else {
    truck.go(payload.rightStick.vert, payload.rightStick.horiz);

    if (payload.upBlueButton) {
      video.moveCamera(payload.leftStick.vert);
      return;
    }

    if (payload.frontSwitch == 0) {
      if (payload.upGreenButton) {
        hand.upHand();
      }
      else {
        hand.operate(payload.leftStick.vert, payload.leftStick.horiz, 1); // altMode
      }
    }
    else if (payload.frontSwitch == 1) {
      if (payload.upGreenButton) {
        hand.upHand();
      }
      else {
        hand.operate(payload.leftStick.vert, payload.leftStick.horiz, 0);
      }
    }
    else if (payload.frontSwitch == 2) {
      if (payload.upGreenButton) {
        tail.upTail();
      }
      else {
        tail.operate(payload.leftStick.vert);
      }
    }
  }
}

bool isAnyProgRunning() {
  return progStairsUp.isRunning()
         || progStairsDown.isRunning()
         /*|| progGoStraight.isRunning()*/
         || progRideTheLine.isRunning();
}

void stopAll() {
  truck.stop();
  hand.stop();
  tail.stop();
  video.stop();
  progStairsUp.stop();
  progStairsDown.stop();
  progRideTheLine.stop();
  //progGoStraight.stop();
}

void tickAll() {
  truck.tick();
  hand.tick();
  tail.tick();
  video.tick();
  //gyro.tick();
  progStairsUp.tick();
  progStairsDown.tick();
  progRideTheLine.tick();
  //progGoStraight.tick();
}
