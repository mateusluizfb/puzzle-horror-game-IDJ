#include "Wall.h"
#include "Log.h"
#include "GameObject.h"
#include "Character.h"
#include "Collider.h"
#include "Game.h"
#include "Pushable.h"
#include "Collision.h"

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
  Collider *collider = other.GetComponent<Collider>();

  if (collider->disabled)
    return;

  if (other.tag != "wall") {
    Collision::ResolveOverlap(other, associated);
  }
}
