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
    hitSound("game/assets/sounds/hit-sound.wav"),
    deathSound("game/audio/Dead.wav"),
    hit(false),
    taskQueue(),
    speed(Vec2(0, 0)),
    linearSpeed(180),
    hp(100),
    deathTimer(Timer()),
    isInvulnerable(false),
    invulnerabilityTimer(Timer())
{
  SpriteRenderer *spriteRenderer = new SpriteRenderer(associated, "game/assets/spritesheets/player.png", 8, 8);
  spriteRenderer->SetScale(2, 2);
  Animator *animator = new Animator(associated);

  associated.AddComponent(spriteRenderer);
  associated.AddComponent(animator);

  animator->AddAnimation("walk_down", Animation(0, 7, 0.2));
  animator->AddAnimation("walk_up", Animation(8, 15, 0.2));
  animator->AddAnimation("walk_right", Animation(16, 23, 0.2));
  animator->AddAnimation("walk_left", Animation(24, 31, 0.2));
  animator->AddAnimation("idle_down", Animation(32, 37, 0.2));
  animator->AddAnimation("idle_up", Animation(40, 45, 0.2));
  animator->AddAnimation("idle_right", Animation(48, 53, 0.2));
  animator->AddAnimation("idle_left", Animation(56, 61, 0.2));

  animator->SetAnimation("idle_down");

  stepSound.Open("game/assets/sounds/step-sound.mp3");
  stepSound.SetVolume(32);
  lastStepFrame = -1;
}

Character::~Character() {
  if (player) {
    player = nullptr;
  }
}

void Character::Start() {
  Log::info("CHARACTER - Starting character component");
  associated.AddComponent(new Collider(associated, Vec2(0.75, 0.75), Vec2(0, 0)));
}

void Character::Update(float dt) {
  Animator* animator = associated.GetComponent<Animator>();

  if (associated.IsDead()) return;

  if (Collider* collider = associated.GetComponent<Collider>()) {
    Vec2 s = collider->GetScale();
    collider->SetOffset(Vec2(
      associated.box.w * (1.0f - s.x) / 2.0f,
      associated.box.h * (1.0f - s.y) / 2.0f));
  }

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
    // animator->SetAnimation("dead");
    deathSound.Play(0);

    if (player) Camera::GetInstance().Unfollow();

    return;
  }

  if (taskQueue.empty() && animator->GetCurrent() != "dead")
  {
    if (speed.x == 0 && speed.y < 0) {
      animator->SetAnimation("idle_up");
    } else if (speed.x == 0 && speed.y > 0) {
      animator->SetAnimation("idle_down");
    } else if (speed.x > 0) {
      animator->SetAnimation("idle_left");
    } else if (speed.x < 0) { 
      animator->SetAnimation("idle_right");
    }
  
    speed = Vec2(0, 0);
    return;
  }
  
  Vec2 moveDir(0, 0);

  while (!taskQueue.empty()) {
      Command item = taskQueue.front();

      switch (item.type)
      {
        case CommandType::MOVE:
        {
          Vec2 cmd = item.pos;
          if (isGlued)
          {
            if (glueAxis == GlueAxis::Horizontal) {
              cmd.y = 0;
            } else if (glueAxis == GlueAxis::Vertical) {
              cmd.x = 0;
            }
          }

          // In case the taskQueue enqueues the same move direction twice,
          // in might happen when pressing two arrows for diagonal movement
          moveDir = moveDir + cmd;
          break;
        }
      }

      taskQueue.pop();
  }

  if (moveDir.x != 0 || moveDir.y != 0) {
    speed = moveDir.Normalize() * linearSpeed;

    if (speed.x == 0 && speed.y < 0) {
      animator->SetAnimation("walk_up");
    } else if (speed.x == 0 && speed.y > 0) {
      animator->SetAnimation("walk_down");
    } else if (speed.x > 0) {
      animator->SetAnimation("walk_right");
    } else if (speed.x < 0) {
      animator->SetAnimation("walk_left");
    }

	// SOM DE PASSOS
	int currentFrame = animator->GetCurrentFrame();
	// Cada animacao tem 6 frames, pegamos o resto da divisao por 6
	int currentColumn = currentFrame % 6;

	// O pe da crianca toca o chao nas colunas 2 e 5 da sua arte
	bool isFootstepFrame = (currentColumn == 2 || currentColumn == 5);

    if (isFootstepFrame && currentFrame != lastStepFrame) {
        stepSound.Play(1);
        lastStepFrame = currentFrame;
    } else if (!isFootstepFrame) {
        lastStepFrame = -1;
    }


    Vec2 moveDelta = speed * dt;
    if (blockDir.x > 0 && moveDelta.x > 0) moveDelta.x = 0;
    if (blockDir.x < 0 && moveDelta.x < 0) moveDelta.x = 0;
    if (blockDir.y > 0 && moveDelta.y > 0) moveDelta.y = 0;
    if (blockDir.y < 0 && moveDelta.y < 0) moveDelta.y = 0;

    associated.box.x += moveDelta.x;
    associated.box.y += moveDelta.y;
  }

  this->SetLinearSpeed(180);
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

  if (other.tag == "monster") {
    this->associated.RequestDelete();
  }

  if (hp <= 0) return;

  hit = true;
}

Vec2 Character::GetCenterPosition() {
  return associated.box.GetCenter();
}

void Character::TakeDamage() {
    if (isInvulnerable) return;

    hitSound.Play(1);
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
