#include "Wall.h"
#include "Log.h"
#include "GameObject.h"
#include "Character.h"
#include "Collider.h"
#include "Game.h"
#include "Pushable.h"

#include <cmath>

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
  Collider *otherCol = other.GetComponent<Collider>();
  Collider *wallCol = associated.GetComponent<Collider>();
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
      other.SetCollisionNormal(Vec2(1, 0));
    }
    else
    {
      other.box.x += overlapRight;
      other.SetCollisionNormal(Vec2(-1, 0));
    }
  }
  else
  {
    if (overlapTop < overlapBottom)
    {
      other.box.y -= overlapTop;
      other.SetCollisionNormal(Vec2(0, 1));
    }
    else
    {
      other.box.y += overlapBottom;
      other.SetCollisionNormal(Vec2(0, -1));
    }
  }
}
