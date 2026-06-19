#include "FollowingMonster.h"
#include "SpriteRenderer.h"
#include "Animator.h"
#include "Animation.h"
#include "Log.h"
#include "Game.h"
#include "Collider.h"
#include "Collision.h"

FollowingMonster::FollowingMonster(GameObject &associated)
  : Component(associated), speed(0, 0)
{
    associated.tag = "monster";

    SpriteRenderer *spriteRenderer = new SpriteRenderer(associated, "game/assets/spritesheets/player.png", 8, 8);
    Animator *animator = new Animator(associated);

    associated.AddComponent(spriteRenderer);
    associated.AddComponent(animator);

    spriteRenderer->SetScale(2, 2);
    
    animator->AddAnimation("walk_down", Animation(0, 7, 0.2));
    animator->AddAnimation("walk_up", Animation(8, 15, 0.2));
    animator->AddAnimation("walk_right", Animation(16, 23, 0.2));
    animator->AddAnimation("walk_left", Animation(24, 31, 0.2));
    animator->AddAnimation("idle_down", Animation(32, 37, 0.2));
    animator->AddAnimation("idle_up", Animation(40, 45, 0.2));
    animator->AddAnimation("idle_right", Animation(48, 53, 0.2));
    animator->AddAnimation("idle_left", Animation(56, 61, 0.2));



    animator->SetAnimation("idle_down");
}

FollowingMonster::~FollowingMonster() {}

void FollowingMonster::Start() {
    Log::info("FOLLOWING_MONSTER - Starting monster component");
}

void FollowingMonster::Update(float dt) {
    Log::info("FOLLOWING_MONSTER - Updating");

    Animator* animator = associated.GetComponent<Animator>();
    if (!animator) return;

    Vec2 blockDir = associated.GetCollisionNormal();
    associated.SetCollisionNormal(Vec2(0, 0));

    // Find player to follow
    GameObject* player = Game::GetInstance().GetCurrentState().GetObjectByTag("player").lock().get();
    if (!player) {
        Log::info("FOLLOWING_MONSTER - Player not found");
        return;
    }

    Vec2 targetPos = player->box.GetCenter();
    Vec2 currentPos = associated.box.GetCenter();
    Vec2 diff = targetPos - currentPos;
    
    Log::info("FOLLOWING_MONSTER - Target: " + std::to_string(targetPos.x) + "," + std::to_string(targetPos.y) + 
               " Current: " + std::to_string(currentPos.x) + "," + std::to_string(currentPos.y));

    Vec2 direction = diff.Normalize();
    speed = direction * linearSpeed;

    if (speed.x == 0 && speed.y < 0) {
        animator->SetAnimation("walk_up");
    } else if (speed.x == 0 && speed.y > 0) {
        animator->SetAnimation("walk_down");
    } else if (speed.x > 0) {
        animator->SetAnimation("walk_right");
    } else if (speed.x < 0) {
        animator->SetAnimation("walk_left");
    }

    Vec2 moveDelta = speed * dt;
    if (blockDir.x > 0 && moveDelta.x > 0) moveDelta.x = 0;
    if (blockDir.x < 0 && moveDelta.x < 0) moveDelta.x = 0;
    if (blockDir.y > 0 && moveDelta.y > 0) moveDelta.y = 0;
    if (blockDir.y < 0 && moveDelta.y < 0) moveDelta.y = 0;

    associated.box.x += moveDelta.x;
    associated.box.y += moveDelta.y;
}

void FollowingMonster::Render() {}
