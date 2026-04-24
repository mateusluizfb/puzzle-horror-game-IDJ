#include "TitleState.h"
#include "GameObject.h"
#include "SpriteRenderer.h"
#include "Log.h"
#include "InputManager.h"
#include "Game.h"
#include "StageState.h"
#include "DebugState.h"
#include "Camera.h"
#include "Text.h"

TitleState::TitleState() : State() {
  Log::info("TITLE_STATE - Initializing TitleState");

  GameObject *titleGameObject = new GameObject();
  titleGameObject->AddComponent(new SpriteRenderer(*titleGameObject, "game/assets/img/Title.png"));
  this->AddObject(titleGameObject);
}

TitleState::~TitleState() {
  Log::info("TITLE_STATE - Destroying TitleState");

  objectArray.clear();
}

void TitleState::LoadAssets() {
  Log::info("TITLE_STATE - Loading assets for TitleState");

  GameObject *textGameObject = new GameObject();
  SDL_Color white = {255, 255, 255, 255};
  Text *text = new Text(*textGameObject, "game/assets/font/neodgm.ttf", 32, Text::BLENDED, "Press SPACE to start", white);
  textGameObject->box.x = (Game::GetInstance().GetWindowWidth() / 2) - 150;
  textGameObject->box.y = (Game::GetInstance().GetWindowHeight() - 100);
  textGameObject->AddComponent(text);
  this->AddObject(textGameObject);
}

void TitleState::Update(float dt) {
  InputManager &inputManager = InputManager::GetInstance();

  if (inputManager.QuitRequested()) {
    Log::warning("TITLE_STATE - Quit requested via SDL event");
    this->RequestQuit();
  }

  if (inputManager.KeyPress(SPACE_KEY))
  {
    Log::info("TITLE_STATE - Enter key pressed, popping TitleState");
    Game::GetInstance().Push(new StageState());
  }

  if (inputManager.KeyPress(SDLK_d))
  {
    Log::info("TITLE_STATE - D key pressed, opening DebugState");
    Game::GetInstance().Push(new DebugState());
  }

  UpdateArray(dt);
}

void TitleState::Render()
{
  RenderArray();
}

void TitleState::Resume() {
  Camera::GetInstance().SetPosition(0, 0);
}

void TitleState::Pause() {}
void TitleState::Start() {}
