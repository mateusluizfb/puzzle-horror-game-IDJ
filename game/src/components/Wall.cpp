#include "Wall.h"
#include "Log.h"
#include "GameObject.h"
#include "Character.h"
#include "Collider.h"
#include "Game.h"
#include "Pushable.h"

#include <cmath>

void ResolveOverlap(GameObject &other, GameObject &wall, Character* character)
{
    Collider *otherCol = other.GetComponent<Collider>();
    Collider *wallCol = wall.GetComponent<Collider>();
    if (!otherCol || !wallCol)
      return;

    Rect &a = otherCol->GetBox();
    Rect &b = wallCol->GetBox();

    float overlapLeft = (a.x + a.w) - b.x;
    float overlapRight = (b.x + b.w) - a.x;
    float overlapTop = (a.y + a.h) - b.y;
    float overlapBottom = (b.y + b.h) - a.y;

    float overlapX = (overlapLeft < overlapRight) ? overlapLeft : overlapRight;
    float overlapY = (overlapTop < overlapBottom) ? overlapTop : overlapBottom;

    if (overlapX < overlapY)
    {
      if (overlapLeft < overlapRight)
      {
        other.box.x -= overlapLeft;
        character->SetCollisionNormal(Vec2(1, 0));
      }
      else
      {
        other.box.x += overlapRight;
        character->SetCollisionNormal(Vec2(-1, 0));
      }
    }
    else
    {
      if (overlapTop < overlapBottom)
      {
        other.box.y -= overlapTop;
        character->SetCollisionNormal(Vec2(0, 1));
      }
      else
      {
        other.box.y += overlapBottom;
        character->SetCollisionNormal(Vec2(0, -1));
      }
    }
}

Wall::Wall(GameObject &associated)
    : Component(associated)
{
  associated.tag = "wall";
}

void Wall::Update(float dt)
{
}

void Wall::Render()
{
}

void Wall::NotifyCollision(GameObject &other)
{
  if (other.tag == "player") {
    Character* character = other.GetComponent<Character>();
    
    if (!character) {
      Log::error("WALL - Player character not found, cannot process collision");
      return;
    }

    ResolveOverlap(other, associated, character);
  }

  if (other.tag == "pushable") {
    Log::info("WALL - Collided with pushable object");

    Pushable* pushable = other.GetComponent<Pushable>();
    if (pushable && !pushable->isPushing)
    {
      Log::info("WALL - Pushable is not being pushed, no collision response needed");
      return;
    }

    std::weak_ptr<GameObject> playerPtr = Game::GetInstance().GetCurrentState().GetObjectByTag("player");
    Character *character = playerPtr.lock()->GetComponent<Character>();

    if (!character)
    {
      Log::error("WALL - Player character not found, cannot process collision with pushable");
      return;
    }

    ResolveOverlap(other, associated, character);
  }
}
