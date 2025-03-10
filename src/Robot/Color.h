#pragma once

#include <Adafruit_TCS34725softi2c.h>

#define COLOR_L_SDA_PIN 30
#define COLOR_L_SCL_PIN 31
#define COLOR_R_SDA_PIN 28
#define COLOR_R_SCL_PIN 29
#define COLOR_LED_PIN 53

class Color {
public:
  void init();
  void enable();
  void disable();

  int getLeft();
  int getRight();

private:
  Adafruit_TCS34725softi2c tcsl = Adafruit_TCS34725softi2c(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_1X, COLOR_L_SDA_PIN, COLOR_L_SCL_PIN);
  Adafruit_TCS34725softi2c tcsr = Adafruit_TCS34725softi2c(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_1X, COLOR_R_SDA_PIN, COLOR_R_SCL_PIN);
};

void Color::init() {
  tcsl.begin();
  tcsr.begin();
  disable();
}

int Color::getLeft() {
  uint16_t r, g, b, c;
  tcsl.getRawData(&r, &g, &b, &c);
  return c;
}

int Color::getRight() {
  uint16_t r, g, b, c;
  tcsr.getRawData(&r, &g, &b, &c);
  return c;
  // return int((double)c * 1.75);
}

void Color::enable() {
  digitalWrite(COLOR_LED_PIN, HIGH);
}

void Color::disable() {
  digitalWrite(COLOR_LED_PIN, LOW);
}
