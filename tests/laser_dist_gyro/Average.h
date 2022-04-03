#pragma once

class Average {
  public:
    void add(int value);
    int getAverage();

  private:
    #define AVERAGE_ACC_COUNT 10
    int _acc[AVERAGE_ACC_COUNT];
};

void Average::add(int value) {
  for (byte i = 0; i < AVERAGE_ACC_COUNT - 1; i++) {
    _acc[i] = _acc[i + 1];
  }
  _acc[AVERAGE_ACC_COUNT - 1] = value;
}

int Average::getAverage() {
  int sum = 0;
  for (int i = 0; i < AVERAGE_ACC_COUNT; i++) {
    sum += _acc[i];
  }
  int average = sum / AVERAGE_ACC_COUNT;
  return average;
}
