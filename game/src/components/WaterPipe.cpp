#include "WaterPipe.h"
#include "Log.h"
#include "GameObject.h"
#include "Character.h"
#include "SpriteRenderer.h"
#include "Animator.h"
#include "InputManager.h"

#include <cmath>

WaterPipe::WaterPipe(GameObject &associated)
    : Component(associated)
{
  associated.tag = "water_pipe";
  SpriteRenderer *spriteRenderer = new SpriteRenderer(associated, "game/assets/tiles/pipes_tileset.png", 6, 5);
  Animator *animator = new Animator(associated);

  associated.AddComponent(spriteRenderer);
  associated.AddComponent(animator);

  animator->AddAnimation("lv0", Animation(18, 18, 0.0));
  animator->AddAnimation("lv1", Animation(19, 19, 0.0));
  animator->AddAnimation("lv2", Animation(20, 20, 0.0));
  animator->AddAnimation("lv3", Animation(21, 21, 0.0));
  animator->AddAnimation("looping", Animation(18, 21, 0.5));

  animator->SetAnimation("lv0");
}

void WaterPipe::LevelUp()
{
  Animator *animator = associated.GetComponent<Animator>();

  if (level == 0) {
    animator->SetAnimation("lv1");
    level = 1;
    return;
  }

  if (level == 1) {
    animator->SetAnimation("lv2");
    level = 2;
    return;
  }

  if (level == 2) {
    animator->SetAnimation("lv3");
    level = 3;
    return;
  }

  if (level == 3) {
    animator->SetAnimation("looping");
    level = 4;
    return;
  }
}

void WaterPipe::LevelDown()
{
  Animator *animator = associated.GetComponent<Animator>();

  if (level == 4) {
    animator->SetAnimation("lv3");
    level = 3;
    return;
  }

  if (level == 3) {
    animator->SetAnimation("lv2");
    level = 2;
    return;
  }

  if (level == 2) {
    animator->SetAnimation("lv1");
    level = 1;
    return;
  }

  if (level == 1) {
    animator->SetAnimation("lv0");
    level = 0;
    return;
  }
}

int WaterPipe::GetLevel()
{
  return level;
}

void WaterPipe::Update(float dt)
{
  InputManager &inputManager = InputManager::GetInstance();
  int mouseWorldX = InputManager::GetInstance().GetMouseXWorld();
  int mouseWorldY = InputManager::GetInstance().GetMouseYWorld();
  Animator *animator = associated.GetComponent<Animator>();

  if (inputManager.MousePress(LEFT_MOUSE_BUTTON) && associated.box.IsVec2Inside(Vec2(mouseWorldX, mouseWorldY)))
  {
    Log::debug("WATER_PIPE - Left mouse button pressed on water pipe at position (" + std::to_string(associated.box.x) + ", " + std::to_string(associated.box.y) + ")");
    this->LevelUp();
  }
}

void WaterPipe::Render()
{
}
