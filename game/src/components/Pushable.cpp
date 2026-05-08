#include "Pushable.h"
#include "Log.h"
#include "GameObject.h"

Pushable::Pushable(GameObject& associated, float pushSpeed, std::string pushableTag)
  : Component(associated),
    pushDirection(Vec2(0, 0)),
    pushSpeed(pushSpeed),
    isPushing(false),
    pushableTag(pushableTag)
{}

void Pushable::Update(float dt) {
  if (isPushing) {
    associated.box.x += pushDirection.x * pushSpeed * dt;
    associated.box.y += pushDirection.y * pushSpeed * dt;
  }
  isPushing = false;
}

void Pushable::Render() {
}

void Pushable::NotifyCollision(GameObject& other) {
  if (other.tag != pushableTag) {
    return;
  }
  
  Vec2 otherCenter = other.box.GetCenter();
  Vec2 selfCenter = associated.box.GetCenter();
  Vec2 collisionNormal = selfCenter - otherCenter;
  
  if (collisionNormal.Magnitude() > 0) {
    pushDirection = collisionNormal.Normalize();
    isPushing = true;
  }
}
