#include "Game.h"
#include "TitleState.h"
#include "EndState.h"
#include "GameData.h"
#include "SpriteRenderer.h"
#include "Log.h"
#include "InputManager.h"

EndState::EndState() : State(), backgroundMusic() {
  Log::info("END_STATE - Initializing EndState");

  if (GameData::playerVictory) {
    GameObject *bgGameObject = new GameObject();
    SpriteRenderer *bgSpriteRenderer = new SpriteRenderer(*bgGameObject, "game/assets/img/Win.png");
    bgGameObject->AddComponent(bgSpriteRenderer);
    this->AddObject(bgGameObject);

    // backgroundMusic.Open("game/audio/endStateWin.ogg");
  } else {
    GameObject *bgGameObject = new GameObject();
    SpriteRenderer *bgSpriteRenderer = new SpriteRenderer(*bgGameObject, "game/assets/img/game_over_screen.png");
    bgSpriteRenderer->SetScale(6.0, 6.0);
    // bgSpriteRenderer->SetPosition(800, 450);
    bgGameObject->AddComponent(bgSpriteRenderer);
    this->AddObject(bgGameObject);

    GameObject *bgTextGameObject = new GameObject();
    SpriteRenderer *bgTextSpriteRenderer = new SpriteRenderer(*bgTextGameObject, "game/assets/img/game_over_text.png");
    bgTextSpriteRenderer->SetScale(3.5, 3.5);
    bgTextSpriteRenderer->SetPosition(300, 200);
    bgGameObject->AddComponent(bgTextSpriteRenderer);
    this->AddObject(bgTextGameObject);

    // backgroundMusic.Open("game/audio/endStateLose.ogg");
  }
}

EndState::~EndState() {
  Log::info("END_STATE - Destroying EndState");

  objectArray.clear();
}

void EndState::LoadAssets() {
  Log::info("END_STATE - Loading assets for EndState");
}

void EndState::Update(float dt) {
  InputManager &inputManager = InputManager::GetInstance();

  if (inputManager.QuitRequested() || inputManager.KeyPress(ESCAPE_KEY))
  {
    Log::warning("END_STATE - Quit requested via SDL event");
    this->RequestQuit();
  }

  if (inputManager.KeyPress(SPACE_KEY)) {
    Log::info("END_STATE - Space key pressed, restarting game");
    popRequested = true;
    Game::GetInstance().Push(new TitleState());
  }

  UpdateArray(dt);
}

void EndState::Render() {
  RenderArray();
}

void EndState::Start() {}
void EndState::Pause() {}
void EndState::Resume() {}
