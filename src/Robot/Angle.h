#pragma once

class Angle {
  public:
    Angle();
    Angle(int startDeg, int minDeg, int maxDeg, int countPointsInDeg = 100);

    void init(int startDeg, int minDeg, int maxDeg, int countPointsInDeg = 100);
    void setMinMax(int minDeg, int maxDeg);
    
    int toDeg();
    int toPoints();
    
    void addDeg(int deg);
    void addPoints(int points);

    void setDeg(int deg);
    void setPoints(int points);

  private:
    int _multiplier;
    int _points;
    int _minPoints;
    int _maxPoints;
};

Angle::Angle(int startDeg, int minDeg, int maxDeg, int countPointsInDeg = 100) {
  init(startDeg, minDeg, maxDeg, countPointsInDeg);
}

Angle::Angle() {
}

void Angle::init(int startDeg, int minDeg, int maxDeg, int countPointsInDeg = 100) {
  _multiplier = countPointsInDeg;
  _points = startDeg * _multiplier;
  _minPoints = minDeg * _multiplier;
  _maxPoints = maxDeg * _multiplier;
}

void Angle::setMinMax(int minDeg, int maxDeg) {
  _minPoints = minDeg * _multiplier;
  _maxPoints = maxDeg * _multiplier;
}

int Angle::toDeg() {
  return _points / _multiplier;
}

int Angle::toPoints() {
  return _points;
}

void Angle::addDeg(int deg) {
  addPoints(deg * _multiplier);
}

void Angle::addPoints(int points) {
  setPoints(_points + points);
}

void Angle::setDeg(int deg) {
  setPoints(deg * _multiplier);
}

void Angle::setPoints(int points) {
  _points = constrain(points, _minPoints, _maxPoints);
}
