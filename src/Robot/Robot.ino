#include "Radio.h"
#include "Truck.h"
#include "Hand.h"
#include "Tail.h"
#include "Camera.h"
#include "ProgStairsUp.h"
#include "ProgStairsDown.h"
#include "ProgGoStraight.h"
#include "Average.h"
#include "Gyro.h"

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

#define CAMERA_FRONT_PIN 12

#define LED_PIN 13

Radio radio;
Truck truck;
Hand hand;
Tail tail;
Camera camera;
Gyro gyro;
ProgStairsUp progStairsUp;
ProgStairsDown progStairsDown;
ProgGoStraight progGoStraight;
unsigned long lastRadioTime = millis();

void setup() {
  Serial.begin(9600);
  radio.initReceiver();
  truck.init(L_EN_PIN, L_INA_PIN, L_INB_PIN, L_PWM_PIN, R_EN_PIN, R_INA_PIN, R_INB_PIN, R_PWM_PIN);
  hand.init(HAND_SHOULDER_PIN, HAND_ELBOW_PIN, HAND_ROTATE_PIN, HAND_CLAW_PIN);
  tail.init(TAIL_COCCYX_PIN);
  camera.init(CAMERA_FRONT_PIN);
  gyro.init();

  progStairsUp.init(truck, tail);
  progStairsDown.init(truck, tail);
  progGoStraight.init(truck, gyro);

  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  truck.tick();
  hand.tick();
  tail.tick();
  camera.tick();
  progStairsUp.tick();
  progStairsDown.tick();
  progGoStraight.tick();

  if (!radio.available()) {
    if (millis() - lastRadioTime > 250) {
      stopAll();
      digitalWrite(LED_PIN, HIGH);
    }

    return;
  }

  digitalWrite(LED_PIN, LOW);
  lastRadioTime = millis();

  Payload payload = radio.read();

  if (payload.frontRedButton) {
    stopAll();
    return;
  }

  if (progStairsUp.isRunning() || progStairsDown.isRunning()) {
    return;
  }

  if (payload.upBlueButton) {
    camera.operate(payload.leftStick.vert);
  }

  else if (payload.frontYellowButton) {
    progStairsUp.start();
  }
  else if (payload.frontWhiteButton) {
    progStairsDown.start();
  }
  else {
    truck.go(payload.rightStick.vert, payload.rightStick.horiz);

    if (payload.frontSwitch == 1) {
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
    else {
      if (payload.upGreenButton) {
        hand.upHand();
      }
      else {
        hand.operate(payload.leftStick.vert, payload.leftStick.horiz, 1);
      }
    }
  }
}

void stopAll() {
  camera.stop();
  truck.stop();
  hand.stop();
  tail.stop();
  progStairsUp.stop();
  progStairsDown.stop();
  progGoStraight.stop();
}
