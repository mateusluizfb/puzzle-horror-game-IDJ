#ifndef PUSHABLE_H
#define PUSHABLE_H

#include <string>
#include "Component.h"
#include "Vec2.h"

class Pushable : public Component {
private:
  Vec2 pushDirection;
  float pushSpeed;

public:
  bool isPushing;

  Pushable(GameObject& associated, float pushSpeed = 100.0f);
  
  void Update(float dt) override;
  void Render() override;
  void NotifyCollision(GameObject& other) override;
};

#endif
