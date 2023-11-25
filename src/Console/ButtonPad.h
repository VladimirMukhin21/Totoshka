#pragma once

#include <Keypad.h>

class ButtonPad {
public:
  byte getKey();

private:
  static const byte ROWS = 4;
  static const byte COLS = 4;

  char hexaKeys[ROWS][COLS] = {
    { 0xA1, 0xB1, 0xC1, 0xD1 },
    { 0xA2, 0xB2, 0xC2, 0xD2 },
    { 0xA3, 0xB3, 0xC3, 0xD3 },
    { 0xA4, 0xB4, 0xC4, 0xD4 }
  };

  byte rowPins[ROWS] = { 19, 18, 17, 16 };
  byte colPins[COLS] = { 20, 21, 22, 23 };

  Keypad keypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
};

byte ButtonPad::getKey() {
  byte key = keypad.getKey();
  if (key == NO_KEY) {
    return 0;
  }

  return key;
}
