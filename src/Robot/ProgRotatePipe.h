#pragma once

#include "Hand.h"

class ProgRotatePipe {
public:
  void init(Hand &hand);
  void start(byte rotateCount = 3);
  void stop();
  void tick();
  bool isRunning();

private:
  enum Phase {
    NONE,
    START,
    CLENCH,
    UNCLENCH,
    CLOCKWIZE,
    COUNTERCLOCKWIZE,
    STOP
  };

  Hand *_hand;
  Phase _phase = NONE;
  byte _rotateCount = 0;
};

void ProgRotatePipe::init(Hand &hand) {
  _hand = &hand;
}

void ProgRotatePipe::start(byte rotateCount = 3) {
  if (!isRunning()) {
    _rotateCount = rotateCount;
    _phase = START;
  }
}

void ProgRotatePipe::stop() {
  _hand->stop();
  _phase = NONE;
}

void ProgRotatePipe::tick() {
  if (_phase == NONE) {
    return;
  }
  else if (_phase == START) {
    _hand->unclench();
    _phase = UNCLENCH;
  }
  else if (_phase == UNCLENCH) {
    if (!_hand->isRunning()) {
      if (_rotateCount-- > 0) {
        _hand->rotate(10);
        _phase = CLOCKWIZE;
      }
      else {
        _hand->rotateToCenter();
        _phase = STOP;
      }
    }
  }
  else if (_phase == CLOCKWIZE) {
    if (!_hand->isRunning()) {
      _hand->clenchPipe();
      _phase = CLENCH;
    }
  }
  else if (_phase == CLENCH) {
    if (!_hand->isRunning()) {
      _hand->rotate(180);
      _phase = COUNTERCLOCKWIZE;
    }
  }
  else if (_phase == COUNTERCLOCKWIZE) {
    if (!_hand->isRunning()) {
      _hand->unclench();
      _phase = UNCLENCH;
    }
  }
  else if (_phase == STOP) {
    if (!_hand->isRunning()) {
      _phase = NONE;
    }
  }
}

bool ProgRotatePipe::isRunning() {
  return _phase != NONE;
}
