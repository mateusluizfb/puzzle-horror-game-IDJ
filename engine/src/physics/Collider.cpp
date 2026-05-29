#ifdef DEBUG
#include "Camera.h"
#include "Game.h"

#include <SDL.h>
#endif // DEBUG

#include "Collider.h"
#include "GameObject.h"

Collider::Collider(GameObject &associated, Vec2 scale, Vec2 offset)
  : Component(associated),
    box(),
    scale(scale),
    offset(offset)
{}

void Collider::Update(float dt) {
  box = Rect(associated.box.x + offset.x,
             associated.box.y + offset.y,
             associated.box.w * scale.x,
             associated.box.h * scale.y);
}

void Collider::Render()
{
#ifdef DEBUG
  Vec2 center(box.GetCenter());
  SDL_Point points[5];

  Vec2 point = (Vec2(box.x, box.y) - center).Rotate(associated.angleDeg / (180 / M_PI)) + center - Camera::GetInstance().pos;
  points[0] = {(int)point.x, (int)point.y};
  points[4] = {(int)point.x, (int)point.y};

  point = (Vec2(box.x + box.w, box.y) - center).Rotate(associated.angleDeg / (180 / M_PI)) + center - Camera::GetInstance().pos;
  points[1] = {(int)point.x, (int)point.y};

  point = (Vec2(box.x + box.w, box.y + box.h) - center).Rotate(associated.angleDeg / (180 / M_PI)) + center - Camera::GetInstance().pos;
  points[2] = {(int)point.x, (int)point.y};

  point = (Vec2(box.x, box.y + box.h) - center).Rotate(associated.angleDeg / (180 / M_PI)) + center - Camera::GetInstance().pos;
  points[3] = {(int)point.x, (int)point.y};

  if (isComposite) {
    SDL_SetRenderDrawColor(Game::GetInstance().GetRenderer(), 255, 0, 255, SDL_ALPHA_OPAQUE);
  } else {
    SDL_SetRenderDrawColor(Game::GetInstance().GetRenderer(), 255, 0, 0, SDL_ALPHA_OPAQUE);
  }
  SDL_RenderDrawLines(Game::GetInstance().GetRenderer(), points, 5);

#endif // DEBUG
}

void Collider::SetScale(Vec2 scale) {
  this->scale = scale;
}

void Collider::SetOffset(Vec2 offset) {
  this->offset = offset;
}

Rect& Collider::GetBox() {
  return box;
}
