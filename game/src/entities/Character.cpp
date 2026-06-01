#include "Log.h"
#include "Game.h"
#include "Character.h"
#include "SpriteRenderer.h"
#include "Animator.h"
#include "Collider.h"
#include "Camera.h"

Character::Command::Command(CommandType type, float x, float y)
  : type(type), pos(x, y) {}

Character::Character(GameObject &associated, std::string sprite)
  : Component(associated),
    player(nullptr),
    hitSound("game/audio/Hit1.wav"),
    deathSound("game/audio/Dead.wav"),
    hit(false),
    taskQueue(),
    speed(Vec2(0, 0)),
    linearSpeed(100),
    hp(100),
    collisionNormal(Vec2(0, 0)),
    deathTimer(Timer())
{
  SpriteRenderer *spriteRenderer = new SpriteRenderer(associated, sprite, 3, 4);
  Animator *animator = new Animator(associated);

  associated.AddComponent(spriteRenderer);
  associated.AddComponent(animator);

  animator->AddAnimation("walking", Animation(0, 5, 0.2));
  animator->AddAnimation("idle", Animation(6, 9, 0.5));
  animator->AddAnimation("dead", Animation(10, 11, 0.5));

  animator->SetAnimation("idle");
}

Character::~Character() {
  if (player) {
    player = nullptr;
  }
}

void Character::Start() {
  Log::info("CHARACTER - Starting character component");
}

void Character::Update(float dt) {
  Animator* animator = associated.GetComponent<Animator>();

  if (associated.IsDead()) return;

  Vec2 blockDir = collisionNormal;
  collisionNormal = Vec2(0, 0);

  if (hit) {
    Timer* hitTimer = &animator->hitTimer;
    hitTimer->Update(dt);

    Log::debug("CHARACTER - Character is hit, time since last hit: " + std::to_string(hitTimer->Get()));

    if (hitTimer->Get() > 0.2) {
      hit = false;
    }
  }

  if (animator->GetCurrent() == "dead" && deathTimer.Get() < 1)
  {
    deathTimer.Update(dt);
    return;
  }

  if (animator->GetCurrent() == "dead" && deathTimer.Get() >= 1)
  {
    Log::info("CHARACTER - Character removal after death animation.");
    associated.RequestDelete();
    return;
  }

  if (hp <= 0 && animator->GetCurrent() != "dead")
  {
    Log::info("CHARACTER - Character is dead.");
    animator->SetAnimation("dead");
    deathSound.Play(1);

    if (player) Camera::GetInstance().Unfollow();

    return;
  }

  if (taskQueue.empty() && animator->GetCurrent() != "dead")
  {
    speed = Vec2(0, 0);
    animator->SetAnimation("idle");
    return;
  }

  while (!taskQueue.empty()) {
      Command item = taskQueue.front();
      
      switch (item.type)
      {
        case CommandType::MOVE:
        {          
          animator->SetAnimation("walking");

          speed = item.pos.Normalize() * linearSpeed;

          SpriteRenderer* spriteRenderer = associated.GetComponent<SpriteRenderer>();

          if (speed.x <= 0) {
            spriteRenderer->SetFlip(SDL_FLIP_HORIZONTAL);
          } else {
            spriteRenderer->SetFlip(SDL_FLIP_NONE);
          }

          Vec2 moveDelta = speed * dt;
          if (blockDir.x > 0 && moveDelta.x > 0) moveDelta.x = 0;
          if (blockDir.x < 0 && moveDelta.x < 0) moveDelta.x = 0;
          if (blockDir.y > 0 && moveDelta.y > 0) moveDelta.y = 0;
          if (blockDir.y < 0 && moveDelta.y < 0) moveDelta.y = 0;

          associated.box.x += moveDelta.x;
          associated.box.y += moveDelta.y;
          break;
        }

        case CommandType::SHOOT:
        {
          break;
        }
      }

      taskQueue.pop();
  }
}

void Character::Render() {
}

void Character::Issue(Command command) {
  taskQueue.push(command);
}

std::queue<Character::Command> Character::GetTaskQueue()
{
  return taskQueue;
}

Vec2 Character::GetSpeed()
{
  return speed;
}

int Character::GetHp() {
  return hp;
}

void Character::NotifyCollision(GameObject &other) {
  if (other.IsDead() || this->associated.IsDead()) return;

  if (other.tag == "wall") {
    Collider* charCol = associated.GetComponent<Collider>();
    Collider* wallCol = other.GetComponent<Collider>();
    if (!charCol || !wallCol) return;

    Rect& a = charCol->GetBox();
    Rect& b = wallCol->GetBox();

    float overlapLeft = (a.x + a.w) - b.x;
    float overlapRight = (b.x + b.w) - a.x;
    float overlapTop = (a.y + a.h) - b.y;
    float overlapBottom = (b.y + b.h) - a.y;

    float overlapX = (overlapLeft < overlapRight) ? overlapLeft : overlapRight;
    float overlapY = (overlapTop < overlapBottom) ? overlapTop : overlapBottom;

    if (overlapX < overlapY) {
      if (overlapLeft < overlapRight) {
        associated.box.x -= overlapLeft;
        collisionNormal = Vec2(1, 0);
      } else {
        associated.box.x += overlapRight;
        collisionNormal = Vec2(-1, 0);
      }
    } else {
      if (overlapTop < overlapBottom) {
        associated.box.y -= overlapTop;
        collisionNormal = Vec2(0, 1);
      } else {
        associated.box.y += overlapBottom;
        collisionNormal = Vec2(0, -1);
      }
    }
    return;
  }

  if (player && IMMORTAL) {
    Log::warning("CHARACTER - IMMORTAL mode active, no damage taken.");
    return;
  };

  if (hp <= 0) return;

  hit = true;
}

Vec2 Character::GetCenterPosition() {
  return associated.box.GetCenter();
}
