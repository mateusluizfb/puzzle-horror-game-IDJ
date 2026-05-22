#include "Wall.h"
#include "Log.h"
#include "GameObject.h"
#include "Character.h"

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
}
