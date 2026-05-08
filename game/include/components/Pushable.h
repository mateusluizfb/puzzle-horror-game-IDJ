#ifndef PUSHABLE_H
#define PUSHABLE_H

#include <string>
#include "Component.h"
#include "Vec2.h"

class Pushable : public Component {
private:
  Vec2 pushDirection;
  float pushSpeed;
  bool isPushing;
  std::string pushableTag;

public:
  Pushable(GameObject& associated, float pushSpeed = 100.0f, std::string pushableTag = "player");
  
  void Update(float dt) override;
  void Render() override;
  void NotifyCollision(GameObject& other) override;
};

#endif
