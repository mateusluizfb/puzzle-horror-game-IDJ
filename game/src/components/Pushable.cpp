#include "Pushable.h"
#include "Log.h"
#include "GameObject.h"
#include "Game.h"
#include "Collider.h"
#include "Character.h"
#include "InputManager.h"
#include "Text.h"
#include "Wall.h"
#include "Collision.h"

#include <cmath>

Pushable::Pushable(GameObject& associated, float pushSpeed)
  : Component(associated),
    pushDirection(Vec2(0, 0)),
    pushSpeed(pushSpeed)
{
  this->associated.tag = "pushable";

  GameObject *textGameObject = new GameObject();
  SDL_Color white = {255, 255, 255, 255};
  pushText = new Text(*textGameObject, "game/assets/font/neodgm.ttf", 32, Text::BLENDED, "Press E to push", white);
  pushText->Hide();
  textGameObject->box.x = (Game::GetInstance().GetWindowWidth() / 2) - 150;
  textGameObject->box.y = (Game::GetInstance().GetWindowHeight() - 100);
  associated.AddComponent(pushText);

  GameObject *textClimbGameObject = new GameObject();
  climbText = new Text(*textClimbGameObject, "game/assets/font/neodgm.ttf", 32, Text::BLENDED, "Press R to climb", white);
  climbText->Hide();
  textClimbGameObject->box.x = (Game::GetInstance().GetWindowWidth() / 2) - 150;
  textClimbGameObject->box.y = (Game::GetInstance().GetWindowHeight() - 60);
  associated.AddComponent(climbText);
}


void Pushable::Update(float dt) {
  if (isTouching && !togglePush && !isClimbed) {
    pushText->Show();
  } else {
    pushText->Hide();
  }

  if (isTouching && !isClimbed) {
    climbText->Show();
  } else {
    climbText->Hide();
  }

  isTouching = false;
}

void Pushable::Render() {}


void Pushable::NotifyCollision(GameObject& other) {
  InputManager &inputManager = InputManager::GetInstance();
  Wall *wall = associated.GetComponent<Wall>();

  if (other.tag == "player") {
    isTouching = true;

    Log::info("PUSHABLE - Collided with player, calculating push direction");
    
    Vec2 blockDir = associated.GetCollisionNormal();
    other.SetCollisionNormal(blockDir);
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

    if (inputManager.KeyPress(E_KEY))
    {
      togglePush = !togglePush;
    }

    if (inputManager.KeyPress(R_KEY))
    {
      isClimbed = !isClimbed;
      togglePush = false; // Ensure we aren't pushing while climbing
    }

    if (isClimbed)
    {
      Collision::KeepWithinBounds(other, associated);
      return; // Skip push logic and overlap resolution
    }

    if (!togglePush && isTouching)
    {
      Collision::ResolveOverlap(other, associated);
      return;
    }

    if (blockDir.x > 0 && direction.x > 0) direction.x = 0;
    if (blockDir.x < 0 && direction.x < 0) direction.x = 0;
    if (blockDir.y > 0 && direction.y > 0) direction.y = 0;
    if (blockDir.y < 0 && direction.y < 0) direction.y = 0;

    associated.box.x += direction.x * pushSpeed * dt;
    associated.box.y += direction.y * pushSpeed * dt;
  };
}
