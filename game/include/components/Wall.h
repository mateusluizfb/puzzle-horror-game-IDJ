#ifndef WALL_H
#define WALL_H

#include <string>
#include "Component.h"
#include "Vec2.h"

class Wall : public Component
{
public:
  Wall(GameObject &associated);

  void Update(float dt) override;
  void Render() override;
  void NotifyCollision(GameObject &other) override;
};

#endif
