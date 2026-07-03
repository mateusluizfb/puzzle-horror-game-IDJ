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
  pushText = new Text(*textGameObject, "game/assets/font/neodgm.ttf", 32, Text::BLENDED, "Press E to push/pull", white);
  pushText->Hide();
  textGameObject->box.x = (Game::GetInstance().GetWindowWidth() / 2) - 150;
  textGameObject->box.y = (Game::GetInstance().GetWindowHeight() - 100);
  associated.AddComponent(pushText);
}


void Pushable::Update(float dt) {
  if (gluedPlayer && gluedPlayer->IsDead()) return;

  if (gluedPlayer) {
    if (glueHorizontal) {
      associated.box.x = gluedPlayer->box.x + glueOffset.x;
    } else {
      associated.box.y = gluedPlayer->box.y + glueOffset.y;
    }

    if (isTouching) {
      glueGrace = GLUE_GRACE_FRAMES;
    } else {
      --glueGrace;
    }

    if (glueGrace <= 0) {
      Character *charComp = gluedPlayer->GetComponent<Character>();
      if (charComp) charComp->isGlued = false;
      gluedPlayer = nullptr;
      togglePush = false;
      pushText->SetText("Press E to push/pull");
    }
  }

  if (isTouching || togglePush) {
    pushText->Show();
  } else {
    pushText->Hide();
  }

  isTouching = false;
}

void Pushable::Render() {}


void Pushable::NotifyCollision(GameObject& other) {
  InputManager &inputManager = InputManager::GetInstance();

  if (other.tag == "pushable")
  {
    Collision::ResolveOverlap(associated, other);
    return;
  }

  if (other.tag == "player") {
    isTouching = true;

    Log::info("PUSHABLE - Collided with player, calculating push direction");
    
    Vec2 blockDir = associated.GetCollisionNormal();
    other.SetCollisionNormal(blockDir);
    associated.SetCollisionNormal(Vec2(0, 0));

    pushDirection = (associated.box.GetCenter() - other.box.GetCenter()).Normalize();

    if (inputManager.KeyPress(E_KEY))
    {
      togglePush = !togglePush;
      if (togglePush) {
        gluedPlayer = &other;
        glueOffset = Vec2(associated.box.x - other.box.x, associated.box.y - other.box.y);
        glueGrace = GLUE_GRACE_FRAMES;
        
        // Use pushDirection instead of blockDir to determine the axis.
        // pushDirection is calculated as (associated.center - other.center).
        if (std::abs(pushDirection.x) > std::abs(pushDirection.y)) {
          glueHorizontal = true;
        } else {
          glueHorizontal = false;
        }
        
        pushText->SetText("Press E to release");
        
        Character* charComp = other.GetComponent<Character>();
        if (charComp) charComp->isGlued = true;
      } else {
        gluedPlayer = nullptr;
        pushText->SetText("Press E to push");
        
        Character* charComp = other.GetComponent<Character>();
        if (charComp) charComp->isGlued = false;
      }
    }


    if (togglePush)
    {
      return;
    }

    if (!togglePush && isTouching)
    {
      Collision::ResolveOverlap(other, associated);
      return;
    }
  };
}
