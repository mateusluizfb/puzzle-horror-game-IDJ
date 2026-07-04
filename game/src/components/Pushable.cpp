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
#include "GlobalSounds.h"

#include <cmath>

Pushable::Pushable(GameObject& associated, float pushSpeed)
  : Component(associated),
    pushDirection(Vec2(0, 0)),
    pushSpeed(pushSpeed)
{
  this->associated.tag = "pushable";

  GameObject *textGameObject = new GameObject();
  SDL_Color white = {255, 255, 255, 255};
  pushText = new Text(*textGameObject, "game/assets/font/neodgm.ttf", 32, Text::BLENDED, "Aperte E para empurrar/puxar", white);
  pushText->Hide();
  textGameObject->box.x = (Game::GetInstance().GetWindowWidth() / 2) - 150;
  textGameObject->box.y = (Game::GetInstance().GetWindowHeight() - 100);
  associated.AddComponent(pushText);
}


void Pushable::Update(float dt) {
  if (gluedPlayer && gluedPlayer->IsDead()) {
    isTouching = false;
    return;
  }

  if (gluedPlayer && togglePush) {
    if (glueAxis == GlueAxis::Horizontal) {
      associated.box.y = glueAnchor;
      gluedPlayer->box.y = playerAnchor;
      associated.box.x = gluedPlayer->box.x + blockOffset;
    } else if (glueAxis == GlueAxis::Vertical) {
      associated.box.x = glueAnchor;
      gluedPlayer->box.x = playerAnchor;
      associated.box.y = gluedPlayer->box.y + blockOffset;
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
    
    pushDirection = (associated.box.GetCenter() - other.box.GetCenter()).Normalize();

    Vec2 blockNormal = associated.GetCollisionNormal();

    if (inputManager.KeyPress(E_KEY))
    {
      GlobalSounds::Button().Play(0);
      togglePush = !togglePush;
      if (togglePush) {
        gluedPlayer = &other;

        if (std::abs(pushDirection.x) > std::abs(pushDirection.y)) {
          glueAxis = GlueAxis::Horizontal;
          glueAnchor = associated.box.y;
          playerAnchor = other.box.y;
          blockOffset = associated.box.x - other.box.x;
        } else {
          glueAxis = GlueAxis::Vertical;
          glueAnchor = associated.box.x;
          playerAnchor = other.box.x;
          blockOffset = associated.box.y - other.box.y;
        }

        pushText->SetText("Aperte E para soltar");

        Character* charComp = other.GetComponent<Character>();
        if (charComp) {
          charComp->isGlued = true;
          charComp->glueAxis = static_cast<Character::GlueAxis>(glueAxis);
        }
      } else {
        gluedPlayer = nullptr;
        glueAxis = GlueAxis::None;
        pushText->SetText("Aperte E para empurrar/puxar");

        Character* charComp = other.GetComponent<Character>();
        if (charComp) {
          charComp->isGlued = false;
          charComp->glueAxis = Character::GlueAxis::None;
        }
      }
    }


    if (togglePush)
    {
      // Glued: propagate blockage to the player only on the glue axis,
      // so the player is halted when the block is wall/pushable-blocked
      // in the push direction, but never on the perpendicular axis.
      if (glueAxis == GlueAxis::Horizontal) {
        other.SetCollisionNormal(Vec2(blockNormal.x, 0));
      } else {
        other.SetCollisionNormal(Vec2(0, blockNormal.y));
      }
      return;
    }

    if (!togglePush && isTouching)
    {
      // Non-glued: if the block is currently blocked by another pushable
      // on the side the player is pushing toward, halt the player on that
      // axis (treat the block as immovable), so the player can't drive A
      // into B.
      if (blockNormal.x != 0 || blockNormal.y != 0) {
        if (std::abs(pushDirection.x) > std::abs(pushDirection.y)) {
          other.SetCollisionNormal(Vec2(blockNormal.x, 0));
        } else {
          other.SetCollisionNormal(Vec2(0, blockNormal.y));
        }
      } else {
        associated.SetCollisionNormal(Vec2(0, 0));
        other.SetCollisionNormal(Vec2(0, 0));
      }
      Collision::ResolveOverlap(other, associated);
      return;
    }
  };
}
