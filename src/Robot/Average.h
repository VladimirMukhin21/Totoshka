#pragma once

class Average {
public:
  Average(bool momentary);
  void add(int value);
  void reset();
  int getAverage();

private:
  static const byte AVERAGE_ACC_COUNT = 10;
  int _acc[AVERAGE_ACC_COUNT];
  long _sum = 0;
  byte _accumulatedCount = 0;

  bool _momentary = false;
  int _preValue = 0;
  unsigned long _getValueTime = 0;
};

Average::Average(bool momentary) {
  _momentary = momentary;
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

  _preValue = 0;
  _getValueTime = 0;
}

int Average::getAverage() {
  if (_accumulatedCount == 0) {
    return 0;
  }

  int average = _sum / _accumulatedCount;

  if (_momentary) {
    return average;
  }

  // величина не моментальная => надо считать как площадь трапеции:  S = (a + b) * h / 2
  unsigned long now = millis();
  int s = _getValueTime > 0
            ? (_preValue + average) * (now - _getValueTime) / 2  // площадь трапеции
            : average;                                           // предыдущего измерения еще нет => площадь нельзя вычислить => вернем текущее среднее

  // запасаем данные для будущих расчетов
  _getValueTime = now;
  _preValue = average;  // сюда запасаем не площадь, а текущее среднее
  return s;             // а клиенту вернем площадь
}
