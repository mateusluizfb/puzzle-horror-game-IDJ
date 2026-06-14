#include "Log.h"
#include "Game.h"
#include "Character.h"
#include "SpriteRenderer.h"
#include "Animator.h"
#include "Collider.h"
#include "Camera.h"
#include "Pushable.h"

Character::Command::Command(CommandType type, float x, float y)
  : type(type), pos(x, y) {}

Character::Character(GameObject &associated, std::string sprite)
  : Component(associated),
    player(nullptr),
    hitSound("game/audio/Hit1.wav"),
    deathSound("game/audio/Dead.wav"),
    hit(false),
	isInvulnerable(false),
    invulnerabilityTimer(Timer()),
    taskQueue(),
    speed(Vec2(0, 0)),
    linearSpeed(400),
    hp(100),
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

  Vec2 blockDir = associated.GetCollisionNormal();
  associated.SetCollisionNormal(Vec2(0, 0));

  if (hit) {
    Timer* hitTimer = &animator->hitTimer;
    hitTimer->Update(dt);

    Log::debug("CHARACTER - Character is hit, time since last hit: " + std::to_string(hitTimer->Get()));

    if (hitTimer->Get() > 0.2) {
      hit = false;
    }
  }

  if (isInvulnerable) {
    invulnerabilityTimer.Update(dt);
    SpriteRenderer* spriteRenderer = associated.GetComponent<SpriteRenderer>();

    if (spriteRenderer != nullptr) {
      // Matematica do piscar (5 = velocidade do piscar)
      if ((int)(invulnerabilityTimer.Get() * 5) % 2 == 0) {
        spriteRenderer->SetColorMod(255, 50, 50); // Fica Vermelho
      } else {
        spriteRenderer->SetColorMod(255, 255, 255); //Fica  Normal
      }
   }

    // Fim do tempo de imunidade (1.5 segundos)
    if (invulnerabilityTimer.Get() > 1.5f) {
      isInvulnerable = false;
      if (spriteRenderer != nullptr) {
        spriteRenderer->SetColorMod(255, 255, 255); // Deixa Normal no final
      }
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

void Character::SetLinearSpeed(float linearSpeed) {
  this->linearSpeed = linearSpeed;
}

float Character::GetLinearSpeed() {
  return linearSpeed;
}

int Character::GetHp() {
  return hp;
}

void Character::NotifyCollision(GameObject &other) {
  if (other.IsDead() || this->associated.IsDead()) return;

  if (other.tag == "wall") {
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

void Character::TakeDamage() {
    if (isInvulnerable) return;

    GameData::playerLives--;
    Log::warning("DANO SOFRIDO! Vidas: " + std::to_string(GameData::playerLives));

    // Ativa o estado exclusivo de imunidade e zera o cronômetro
    isInvulnerable = true;
    invulnerabilityTimer.Restart();

    // Logica de morte
    if (GameData::playerLives <= 0) {
        associated.RequestDelete();
        GameData::playerLives = 3;
    }
}
