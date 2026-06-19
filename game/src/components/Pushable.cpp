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
  if (gluedPlayer) {
    // Check if player has moved too far sideways to maintain the glue
    float distY = std::abs(gluedPlayer->box.GetCenter().y - associated.box.GetCenter().y);
    float distX = std::abs(gluedPlayer->box.GetCenter().x - associated.box.GetCenter().x);
    float combinedHeight = (gluedPlayer->box.h + associated.box.h) * 0.75f;
    float combinedWidth = (gluedPlayer->box.w + associated.box.w) * 0.75f;

    if ((glueHorizontal && distY > combinedHeight) || (!glueHorizontal && distX > combinedWidth)) {
      gluedPlayer = nullptr;
      togglePush = false;
      pushText->SetText("Press E to push");
    } else {
      if (glueHorizontal) {
        associated.box.x = gluedPlayer->box.x + glueOffset.x;
      } else {
        associated.box.y = gluedPlayer->box.y + glueOffset.y;
      }
    }
  }

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

    float dt = Game::GetInstance().GetDeltaTime();

    if (inputManager.KeyPress(E_KEY))
    {
      togglePush = !togglePush;
      if (togglePush) {
        gluedPlayer = &other;
        glueOffset = Vec2(associated.box.x - other.box.x, associated.box.y - other.box.y);
        
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
