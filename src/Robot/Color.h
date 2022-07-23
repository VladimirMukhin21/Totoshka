#pragma once

#include <Adafruit_TCS34725softi2c.h>

#define SDApinl 20
#define SCLpinl 21

#define SDApinr 30
#define SCLpinr 31

class Color {
  public:
    void init();
    void tick();

    int getLeft();
    int getRight();

  private:
    Adafruit_TCS34725softi2c tcsl = Adafruit_TCS34725softi2c(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_1X, SDApinl, SCLpinl);
    Adafruit_TCS34725softi2c tcsr = Adafruit_TCS34725softi2c(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_1X, SDApinr, SCLpinr);
};

void Color::init() {
  tcsl.begin();
  tcsr.begin();
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
}

void Color::tick() {

}
