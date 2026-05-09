#ifndef REACT_H
#define REACT_H

#include "Vec2.h"

class Rect {
public:
  float x, y, w, h;

  Rect();
  Rect(float x, float y, float w, float h);

  Rect operator+(const Vec2 &other) const;
  Rect operator-(const Vec2 &other) const;

  Vec2 GetCenter() const;
  float Distance(const Rect &other) const;
  bool IsVec2Inside(const Vec2 &vec) const;
  void SetCenter(Vec2 newCenter);
};

#endif
