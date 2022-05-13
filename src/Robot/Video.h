#pragma once

#include "Camera.h"

class Video {
  public:
    void init(byte switcherPin, byte frontServoPin, byte topServoPin);
    void setActiveCamera(byte activeCam);
    void moveCamera(byte stickVert);
    void stop();
    void tick();

  private:
    byte _switcherPin;
    byte _activeCam = 0;
    Camera _front;
    Camera _top;
};

void Video::init(byte switcherPin, byte frontServoPin, byte topServoPin) {
  _switcherPin = switcherPin;
  _front.init(frontServoPin, 0, 0, 180, 25);
  _top.init(topServoPin, 90, 0, 180, 25);
  pinMode(_switcherPin, OUTPUT);
}

void Video::setActiveCamera(byte activeCam) {
  if (_activeCam == activeCam) {
    return;
  }

  _activeCam = activeCam;
  switch (_activeCam) {
    case 0:
      analogWrite(_switcherPin, 0);
      break;
    case 1:
      analogWrite(_switcherPin, 127);
      break;
  }
}

void Video::moveCamera(byte stickVert) {
  switch (_activeCam) {
    case 0:
      _front.operate(stickVert);
      break;
    case 1:
      _top.operate(stickVert);
      break;
  }
}

void Video::stop() {
  _front.stop();
  _top.stop();
}

void Video::tick() {
  _front.tick();
  _top.tick();
}
