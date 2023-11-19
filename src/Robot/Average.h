#pragma once

class Average {
  public:
    Average();
    void add(int value);
    void reset();
    int getAverage();

  private:
#define AVERAGE_ACC_COUNT 100
    int _acc[AVERAGE_ACC_COUNT];
    int _sum = 0;
    byte _accumulatedCount = 0;
};

Average::Average() {
  reset();
}

void Average::add(int value) {
  _sum -= _acc[0];
  _sum += value;

  if (_accumulatedCount < AVERAGE_ACC_COUNT) {
    _accumulatedCount++;
  }

  for (byte i = 1; i < AVERAGE_ACC_COUNT; i++) {
    _acc[i - 1] = _acc[i];
  }

  _acc[AVERAGE_ACC_COUNT - 1] = value;
}

void Average::reset() {
  _sum = 0;
  _accumulatedCount = 0;
  for (byte i = 0; i < AVERAGE_ACC_COUNT; i++) {
    _acc[i] = 0;
  }
}

int Average::getAverage() {
  if (_accumulatedCount == 0) {
    return 0;
  }

  int average = _sum / _accumulatedCount;
  return average;
}
