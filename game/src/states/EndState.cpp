#include "Game.h"
#include "TitleState.h"
#include "EndState.h"
#include "GameData.h"
#include "SpriteRenderer.h"
#include "Log.h"
#include "InputManager.h"
#include "Sound.h"
#include "Timer.h"
#include "GameObject.h"

EndState::EndState() : State(), backgroundMusic() {
  Log::info("END_STATE - Initializing EndState");

  if (GameData::playerVictory) {
    GameObject *bgGameObject = new GameObject();
    SpriteRenderer *bgSpriteRenderer = new SpriteRenderer(*bgGameObject, "game/assets/img/victory_screen.png");
    bgSpriteRenderer->SetScale(6.0, 6.0);
    bgGameObject->AddComponent(bgSpriteRenderer);
    this->AddObject(bgGameObject);

    GameObject *bgTextGameObject = new GameObject();
    SpriteRenderer *bgTextSpriteRenderer = new SpriteRenderer(*bgTextGameObject, "game/assets/img/victory_text.png");
    bgTextSpriteRenderer->SetScale(3.5, 3.5);
    bgTextSpriteRenderer->SetPosition(600, 750);
    bgGameObject->AddComponent(bgTextSpriteRenderer);
    this->AddObject(bgTextGameObject);

    // backgroundMusic.Open("game/audio/endStateWin.ogg");
  } else {
    GameObject *bgGameObject = new GameObject();
    SpriteRenderer *bgSpriteRenderer = new SpriteRenderer(*bgGameObject, "game/assets/img/game_over_screen.png");
    bgSpriteRenderer->SetScale(6.0, 6.0);
    bgSpriteRenderer->hidden = true;
    bgGameObject->AddComponent(bgSpriteRenderer);
    this->AddObject(bgGameObject);
    this->gameOverScreen = this->GetObjectPtr(bgGameObject);

    GameObject *bgTextGameObject = new GameObject();
    SpriteRenderer *bgTextSpriteRenderer = new SpriteRenderer(*bgTextGameObject, "game/assets/img/game_over_text.png");
    bgTextSpriteRenderer->SetScale(3.5, 3.5);
    bgTextSpriteRenderer->SetPosition(300, 200);
    bgTextSpriteRenderer->hidden = true;
    bgTextGameObject->AddComponent(bgTextSpriteRenderer);
    this->AddObject(bgTextGameObject);
    this->gameOverText = this->GetObjectPtr(bgTextGameObject);

    GameObject *exorcistGo = new GameObject();
    SpriteRenderer *exorcistSpriteRenderer = new SpriteRenderer(*exorcistGo, "game/assets/img/exorcist.png");
    exorcistSpriteRenderer->SetCameraFollower(true);
    exorcistSpriteRenderer->SetScale(1.75, 1.75);
    exorcistSpriteRenderer->hidden = true;
    exorcistGo->AddComponent(exorcistSpriteRenderer);
    this->AddObject(exorcistGo);
    this->exorcistGameObject = this->GetObjectPtr(exorcistGo);

    scream = Sound("game/audio/scream.mp3");
    scream.SetVolume(32);

    jumpscareActive = true;
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

  if (jumpscareActive)
  {
    jumpscareTimer.Update(dt);
    if (jumpscareTimer.Get() >= 2.0f)
    {
      Log::info("END_STATE - Jumpscare finished, revealing game over screen");

      auto exorcistGo = this->exorcistGameObject.lock();
      if (exorcistGo)
      {
        SpriteRenderer *exorcistSpriteRenderer = exorcistGo->GetComponent<SpriteRenderer>();
        if (exorcistSpriteRenderer != nullptr)
        {
          exorcistSpriteRenderer->hidden = true;
        }
      }

      auto screenGo = this->gameOverScreen.lock();
      if (screenGo)
      {
        SpriteRenderer *screenSpriteRenderer = screenGo->GetComponent<SpriteRenderer>();
        if (screenSpriteRenderer != nullptr)
        {
          screenSpriteRenderer->hidden = false;
        }
      }

      auto textGo = this->gameOverText.lock();
      if (textGo)
      {
        SpriteRenderer *textSpriteRenderer = textGo->GetComponent<SpriteRenderer>();
        if (textSpriteRenderer != nullptr)
        {
          textSpriteRenderer->hidden = false;
        }
      }

      jumpscareActive = false;
    }
  }

  if (!jumpscareActive)
  {
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
  }

  UpdateArray(dt);
}

void EndState::Render() {
  RenderArray();
}

void EndState::Start() {
  if (jumpscareActive)
  {
    Log::info("END_STATE - Starting death jumpscare sequence");
    jumpscareTimer.Restart();

    auto exorcistGo = this->exorcistGameObject.lock();
    if (exorcistGo)
    {
      SpriteRenderer *exorcistSpriteRenderer = exorcistGo->GetComponent<SpriteRenderer>();
      if (exorcistSpriteRenderer != nullptr)
      {
        exorcistSpriteRenderer->hidden = false;
      }
    }
    scream.Play(0);
  }
}
void EndState::Pause() {}
void EndState::Resume() {}
