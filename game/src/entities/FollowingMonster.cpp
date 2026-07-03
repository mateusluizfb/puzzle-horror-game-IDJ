#include "FollowingMonster.h"
#include "SpriteRenderer.h"
#include "Animator.h"
#include "Animation.h"
#include "Log.h"
#include "Game.h"
#include "Collider.h"
#include "Collision.h"
#include "InputManager.h"

FollowingMonster::FollowingMonster(GameObject &associated)
  : Component(associated), speed(0, 0)
{
    associated.tag = "monster";

    SpriteRenderer *spriteRenderer = new SpriteRenderer(associated,
			"game/assets/spritesheets/Kramulhao.png", 8, 4);
    Animator *animator = new Animator(associated);

    spriteRenderer->SetPosition(1100, 850);

    associated.AddComponent(spriteRenderer);
    associated.AddComponent(animator);

    spriteRenderer->SetScale(2, 2);
    
    animator->AddAnimation("walk_down", Animation(0, 7, 0.2));
    animator->AddAnimation("walk_up", Animation(8, 15, 0.2));
    animator->AddAnimation("walk_right", Animation(16, 23, 0.2));
    animator->AddAnimation("walk_left", Animation(24, 31, 0.2));

    animator->SetAnimation("walk_up");
}

FollowingMonster::~FollowingMonster() {}

void FollowingMonster::Start() {
    Log::info("FOLLOWING_MONSTER - Starting monster component");
}

void FollowingMonster::FollowPlayer(float dt, Animator *animator, bool checkProximity) {
    Vec2 blockDir = associated.GetCollisionNormal();
    associated.SetCollisionNormal(Vec2(0, 0));

    // Find player to follow
    GameObject *player = Game::GetInstance().GetCurrentState().GetObjectByTag("player").lock().get();
    if (!player) {
        return;
    }

    Vec2 targetPos = player->box.GetCenter();
    Vec2 currentPos = associated.box.GetCenter();
    Vec2 diff = targetPos - currentPos;

    Vec2 direction = diff.Normalize();
    speed = direction * linearSpeed;

	float absX = std::abs(speed.x);
	float absY = std::abs(speed.y);

	if (absX > absY) {
		animator->SetAnimation(speed.x > 0 ? "walk_right" : "walk_left");
	} else {
		animator->SetAnimation(speed.y > 0 ? "walk_down" : "walk_up");
	}

    Vec2 moveDelta = speed * dt;
    if (blockDir.x > 0 && moveDelta.x > 0) moveDelta.x = 0;
    if (blockDir.x < 0 && moveDelta.x < 0) moveDelta.x = 0;
    if (blockDir.y > 0 && moveDelta.y > 0) moveDelta.y = 0;
    if (blockDir.y < 0 && moveDelta.y < 0) moveDelta.y = 0;

    associated.box.x += moveDelta.x;
    associated.box.y += moveDelta.y;

    if (checkProximity) {
        float distance = associated.box.GetCenter().Distance(player->box.GetCenter());
        if (distance <= 85.0f) {
            flickerTimer.Restart();
            currentState = (currentState == State::Stage1) ? State::Flicker1 : State::Flicker2;
        }
    }
}

void FollowingMonster::Update(float dt) {
    Animator *animator = associated.GetComponent<Animator>();
    if (!animator) return;

    switch (currentState) {
        case State::Stage1:
            FollowPlayer(dt, animator, true);
            break;

        case State::Stage2:
            FollowPlayer(dt, animator, true);
            break;

        case State::Stage3:
            FollowPlayer(dt, animator, false);
            break;

        case State::Flicker1:
        case State::Flicker2: {
            flickerTimer.Update(dt);

            // Vibration: jitter a few pixels in random directions each frame
            int jitterX = (rand() % 7) - 3;   // -3 .. +3
            int jitterY = (rand() % 7) - 3;
            associated.box.x += jitterX;
            associated.box.y += jitterY;

            if (flickerTimer.Get() >= 1.0f) {
                if (currentState == State::Flicker1) {
                    associated.box.SetCenter(Vec2(1100, 25));
                    currentState = State::Stage2;
                } else {
                    associated.box.SetCenter(Vec2(0, 0));
                    currentState = State::Stage3;
                }
            }
            break;
        }
    }
}

void FollowingMonster::Render() {}
