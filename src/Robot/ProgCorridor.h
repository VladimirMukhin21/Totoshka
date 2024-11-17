// Изломанный коридор.
// Необходимо проехать по коридору, состоящему из белых стенок.
// На полу не будет нанесено дополнительных линий, робот может ориентироваться только на изгибы этих самых стенок.
// Высота стен – 400мм, расстояние между стенками – 800мм. Стены коридора изгибаются под 90 град.
// Коридор состоит из трех ячеек. Автоматически останавливаться не требуется.

#pragma once

#include "Truck.h"
#include "DistMeter.h"

class ProgCorridor {
public:
  void init(Truck &truck, DistMeter &distMeter);
  void start(int8_t turn);
  void stop();
  void tick();
  bool isRunning();

  static const int8_t TURN_LEFT = -1;
  static const int8_t TURN_RIGHT = 1;

private:
  enum Phase {
    NONE,
    STARTING,
    DRIVE,
    SLOW_DRIVE,
    TURN_90,
    TURN_180
  };

  static const int DRIVE_SPEED = 70;          // скорость езды
  static const int SLOW_DRIVE_SPEED = 70;     // скорость медленного подъезда до стены
  static const int DIST_TO_SLOW_DRIVE = 150;  // расстояние до стены, на котором снижаем скорость
  static const int DIST_TO_TURN = 75;         // расстояние до стены, на котором останавливаемся и поворачиваем
  static const int DIST_TO_180_TURN = 300;    // если после поворота расстояние до стены меньше этого значения, то не угадали в какую сторону поворачиваться и надо развернуться на 180
  static const int TURN_ANGLE = 90;           // угол, на который поворачивается робот (четверть окружности)

  Truck *_truck;
  DistMeter *_distMeter;
  Phase _phase = NONE;
  int8_t _turnMultiplier = TURN_LEFT;

  unsigned long _tickTime = millis();
};

void ProgCorridor::init(Truck &truck, DistMeter &distMeter) {
  _truck = &truck;
  _distMeter = &distMeter;
}

void ProgCorridor::start(int8_t turn) {
  if (!isRunning()) {
    _phase = STARTING;
    _turnMultiplier = turn;
  }
}

void ProgCorridor::stop() {
  _truck->stop();
  _distMeter->disable();
  _phase = NONE;
  //Serial.println("stop");
}

void ProgCorridor::tick() {
  if (!isRunning()) {
    return;
  }

  if (_phase == STARTING) {
    _truck->goStraight(DRIVE_SPEED, false);
    _phase = DRIVE;
  }
  else if (_phase == DRIVE) {
    // едем вперед
    int dist = _distMeter->getDist();
    if (dist <= DIST_TO_SLOW_DRIVE) {
      // подъехали к стене близко => снижаем скорость, девиацию не сбрасываем
      _truck->goStraight(SLOW_DRIVE_SPEED, false);
      _phase = SLOW_DRIVE;
    }
  }
  else if (_phase == SLOW_DRIVE) {
    // медленно подъезжаем к стене
    int dist = _distMeter->getDist();
    if (dist <= DIST_TO_TURN) {
      // доехали до стены => поворачиваем
      _truck->stop(Motor::SMOOTH_OFF);
      _truck->turn(_turnMultiplier * TURN_ANGLE);
      _phase = TURN_90;
    }
  }
  else if (_phase == TURN_90) {
    if (!_truck->isRunning()) {
      // повернули на 90 => измеряем расстояние, проверям в правильную ли сторону повернули
      int dist = _distMeter->getDist();
      if (dist <= DIST_TO_180_TURN) {
        // стена близко => не угадали в какую сторону повернуться => разворачиваемся на 180
        _truck->turn(2 * _turnMultiplier * TURN_ANGLE);
        _phase = TURN_180;
      }
      else {
        // стена далеко => едем
        _truck->goStraight(DRIVE_SPEED, false);
        _phase = DRIVE;
      }
    }
  }
  else if (_phase == TURN_180) {
    if (!_truck->isRunning()) {
      // развернулись на 180 => измеряем расстояние
      int dist = _distMeter->getDist();
      if (dist <= DIST_TO_180_TURN) {
        // стена опять близко, непонятно что делать => стоп
        stop();
      }
      else {
        // стена далеко => едем
        _truck->goStraight(DRIVE_SPEED, false);
        _phase = DRIVE;
      }
    }
  }
}

bool ProgCorridor::isRunning() {
  return _phase != NONE;
}
