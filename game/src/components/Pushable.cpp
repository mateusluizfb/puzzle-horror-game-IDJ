#include "Pushable.h"
#include "Log.h"
#include "GameObject.h"
#include "Game.h"
#include "Collider.h"

#include <cmath>

Pushable::Pushable(GameObject& associated, float pushSpeed)
  : Component(associated),
    pushDirection(Vec2(0, 0)),
    pushSpeed(pushSpeed),
    isPushing(false)
{
  this->associated.tag = "pushable";
}

void Pushable::Update(float dt) {
  isPushing = false;
}

void Pushable::Render() {}

void Pushable::NotifyCollision(GameObject& other) {
  if (other.tag == "player") {
    Log::info("PUSHABLE - Collided with player, calculating push direction");
    
    Vec2 blockDir = associated.GetCollisionNormal();
    associated.SetCollisionNormal(Vec2(0, 0));

    pushDirection = (associated.box.GetCenter() - other.box.GetCenter()).Normalize();

    Vec2 direction = Vec2(0, 0);

    if (std::abs(pushDirection.x) > std::abs(pushDirection.y))
    {
      direction.x = (pushDirection.x > 0) ? 1.0f : -1.0f;
    }
    else
    {
      direction.y = (pushDirection.y > 0) ? 1.0f : -1.0f;
    }

    float dt = Game::GetInstance().GetDeltaTime();

    if (blockDir.x > 0 && direction.x > 0) direction.x = 0;
    if (blockDir.x < 0 && direction.x < 0) direction.x = 0;
    if (blockDir.y > 0 && direction.y > 0) direction.y = 0;
    if (blockDir.y < 0 && direction.y < 0) direction.y = 0;

    associated.box.x += direction.x * pushSpeed * dt;
    associated.box.y += direction.y * pushSpeed * dt;

    isPushing = true;
  };
}
