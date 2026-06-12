#include "Pushable.h"
#include "Log.h"
#include "GameObject.h"
#include "Game.h"

#include <cmath>

Pushable::Pushable(GameObject& associated, float pushSpeed)
  : Component(associated),
    pushDirection(Vec2(0, 0)),
    pushSpeed(pushSpeed),
    isPushing(false)
{}

void Pushable::Update(float dt) {
  (void)dt;
  isPushing = false;
}

void Pushable::Render() {}

void Pushable::NotifyCollision(GameObject& other) {
  if (other.tag != "player") return;

  Vec2 collisionNormal = associated.box.GetCenter() - other.box.GetCenter();
  
  pushDirection = collisionNormal.Normalize();
  
  Vec2 direction = Vec2(0, 0);

  if (std::abs(pushDirection.x) > std::abs(pushDirection.y)) {
    direction.x = (pushDirection.x > 0) ? 1.0f : -1.0f;
  } else {
    direction.y = (pushDirection.y > 0) ? 1.0f : -1.0f;
  }

  float dt = Game::GetInstance().GetDeltaTime();
  associated.box.x += direction.x * pushSpeed * dt;
  associated.box.y += direction.y * pushSpeed * dt;
  
  isPushing = true;
}
