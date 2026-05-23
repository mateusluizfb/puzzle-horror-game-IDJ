#include "Rect.h"

#include <algorithm>

Rect::Rect() : x(0.0), y(0.0), w(0.0), h(0.0) {}

Rect::Rect(float x, float y, float w, float h) : x(x), y(y), w(w), h(h) {}

Rect Rect::operator+(const Vec2 &other) const
{
  return Rect(this->x + other.x, this->y + other.y, this->w, this->h);
}

Rect Rect::operator-(const Vec2 &other) const
{
  return Rect(this->x - other.x, this->y - other.y, this->w, this->h);
}

Vec2 Rect::GetCenter() const
{
  return Vec2(this->x + this->w / 2.0f, this->y + this->h / 2.0f);
}

float Rect::Distance(const Rect &other) const
{
  Vec2 center1 = this->GetCenter();
  Vec2 center2 = other.GetCenter();

  return center1.Distance(center2);
}

bool Rect::IsVec2Inside(const Vec2 &vec) const
{
  return (this->x < vec.x) &&
         (this->y < vec.y) &&
         (vec.x < (this->x + this->w)) &&
         (vec.y < (this->y + this->h));
}

void Rect::SetCenter(Vec2 newCenter)
{
  this->x = newCenter.x - (this->w / 2.0f);
  this->y = newCenter.y - (this->h / 2.0f);
}

bool Rect::Overlaps(const Rect &a, const Rect &b)
{
  return (a.x <= b.x + b.w &&
          a.x + a.w >= b.x &&
          a.y <= b.y + b.h &&
          a.y + a.h >= b.y);
}

Rect Rect::Merge(const Rect &a, const Rect &b)
{
  float x = std::min(a.x, b.x);
  float y = std::min(a.y, b.y);
  float max_x = std::max(a.x + a.w, b.x + b.w);
  float max_y = std::max(a.y + a.h, b.y + b.h);
  return Rect(x, y, max_x - x, max_y - y);
}
