#ifndef PUSHABLE_H
#define PUSHABLE_H
 
#include <string>
#include "Component.h"
#include "Vec2.h"
#include "Text.h"
#include "GameObject.h"
 
class Pushable : public Component {
 private:
   static constexpr int GLUE_GRACE_FRAMES = 2;
   int glueGrace = 0;
   Vec2 pushDirection;
   float pushSpeed;
   Text *pushText = nullptr;
  
 public:
  bool isTouching = false; 
  bool togglePush = false;
  GameObject* gluedPlayer = nullptr;
  Vec2 glueOffset = Vec2(0, 0);
  bool glueHorizontal = false;
 
  Pushable(GameObject& associated, float pushSpeed = 100.0f);
  
  void Update(float dt) override;
  void Render() override;
  void NotifyCollision(GameObject& other) override;
};
 
#endif

