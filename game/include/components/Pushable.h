#ifndef PUSHABLE_H
#define PUSHABLE_H
 
#include <string>
#include "Component.h"
#include "Vec2.h"
#include "Text.h"
#include "GameObject.h"
 
class Pushable : public Component {
 private:
   Vec2 pushDirection;
   float pushSpeed;
   Text *pushText = nullptr;
 
 public:
  enum class GlueAxis { None, Horizontal, Vertical };

  bool isTouching = false; 
  bool togglePush = false;
  GameObject* gluedPlayer = nullptr;
  GlueAxis glueAxis = GlueAxis::None;
  float glueAnchor = 0.0f;
  float playerAnchor = 0.0f;
  float blockOffset = 0.0f;
 
  Pushable(GameObject& associated, float pushSpeed = 100.0f);
  
  void Update(float dt) override;
  void Render() override;
  void NotifyCollision(GameObject& other) override;
};
 
#endif