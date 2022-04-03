#pragma once

class Average {
  public:
    void add(int value);
    int getAverage();

  private:
    #define AVERAGE_ACC_COUNT 10
    int _acc[AVERAGE_ACC_COUNT];
    int _sum = 0;
    byte _accumulatedCount = 0;
};

void Average::add(int value) {
  _sum -= _acc[0];
  _sum += value;
  
  if (_accumulatedCount < AVERAGE_ACC_COUNT) {
    _accumulatedCount++;
  }
  
  for (byte i = 0; i < AVERAGE_ACC_COUNT - 1; i++) {
    _acc[i] = _acc[i + 1];
  }
  
  _acc[AVERAGE_ACC_COUNT - 1] = value;
}

int Average::getAverage() {
  if (_accumulatedCount == 0) {
    return 0;
  }
  
  int average = _sum / _accumulatedCount;
  return average;
}
