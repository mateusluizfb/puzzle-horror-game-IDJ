#include "Rat.h"
#include "SpriteRenderer.h"
#include "Animator.h"
#include "Animation.h"
#include "Collider.h"
#include "Game.h"
#include "InputManager.h"
#include "GameData.h"
#include "QuizState.h"
#include "Log.h"

Rat::Rat(GameObject &associated, int x, int y)
    : Component(associated)
{
    associated.tag = "rat";

    SpriteRenderer *spriteRenderer = new SpriteRenderer(associated,
            "game/assets/spritesheets/rat.png", 8, 2);
    Animator *animator = new Animator(associated);
    Collider *collider = new Collider(associated, Vec2(1, 1), Vec2(0, 0));
    collider->disabled = true;

    spriteRenderer->SetPosition(x, y);
    spriteRenderer->SetScale(2, 2);

    associated.AddComponent(spriteRenderer);
    associated.AddComponent(animator);
    associated.AddComponent(collider);

    animator->AddAnimation("idle", Animation(0, 15, 0.2));
    animator->SetAnimation("idle");

    SDL_Color white = {255, 255, 255, 255};

    GameObject *textGameObject = new GameObject();
    promptText = new Text(*textGameObject, "game/assets/font/neodgm.ttf", 32, Text::BLENDED, "Aperte E para conversar", white);
    promptText->Hide();
    textGameObject->box.x = (Game::GetInstance().GetWindowWidth() / 2) - 150;
    textGameObject->box.y = (Game::GetInstance().GetWindowHeight() - 100);
    associated.AddComponent(promptText);
}

Rat::~Rat() {}

void Rat::Start() {
    Log::info("RAT - Starting rat component");
}

void Rat::Update(float /*dt*/) {
    if (isTouching) {
        promptText->Show();
    } else {
        promptText->Hide();
    }

    isTouching = false;
    triggered = false;
}

void Rat::Render() {}

void Rat::NotifyCollision(GameObject &other) {
    if (other.tag != "player" || triggered) return;

    isTouching = true;

    InputManager &inputManager = InputManager::GetInstance();

    if (inputManager.KeyPress(E_KEY) && !GameData::dialogueActive) {
        Log::info("RAT - Pushing QuizState");
        triggered = true;
        QuizState *quizState = new QuizState();
        quizState->SetIsOverlay();
        Game::GetInstance().Push(quizState);
    }
}
