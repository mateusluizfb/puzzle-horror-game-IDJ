#include "TitleState.h"
#include "GameObject.h"
#include "SpriteRenderer.h"
#include "Log.h"
#include "InputManager.h"
#include "Game.h"
#include "BedroomState.h"
#include "Camera.h"
#include "Text.h"
#include "StageState.h"
#include "EndState.h"
#include "GameData.h"
#include "IntroState.h"

TitleState::TitleState() : State(), music("game/assets/music/soundtrack.wav")
{
  Log::info("TITLE_STATE - Initializing TitleState");
}

TitleState::~TitleState() {
  Log::info("TITLE_STATE - Destroying TitleState");

  objectArray.clear();
}

void TitleState::LoadAssets() {
  Log::info("TITLE_STATE - Loading assets for TitleState");

  GameObject *titleGameObject = new GameObject();
  SpriteRenderer *titleSpriteRenderer = new SpriteRenderer(*titleGameObject, "game/assets/img/title_screen.png");
  titleSpriteRenderer->SetScale(0.6, 0.6);
  titleGameObject->AddComponent(titleSpriteRenderer);
  this->AddObject(titleGameObject);

  GameObject *titleTextGameObject = new GameObject();
  SpriteRenderer *titleTextSpriteRenderer = new SpriteRenderer(*titleTextGameObject, "game/assets/img/title_text.png");
  titleTextSpriteRenderer->SetScale(3.5, 3.5);
  titleTextSpriteRenderer->SetPosition(800, 450);
  titleTextGameObject->AddComponent(titleTextSpriteRenderer);
  this->AddObject(titleTextGameObject);

  GameObject *textGameObject = new GameObject();
  SDL_Color white = {255, 255, 255, 255};
  Text *text = new Text(*textGameObject, "game/assets/font/neodgm.ttf", 32, Text::BLENDED, "Aperte ESPAÇO para iniciar", white);
  textGameObject->box.x = (Game::GetInstance().GetWindowWidth() / 2) - 250;
  textGameObject->box.y = (Game::GetInstance().GetWindowHeight() - 100);
  textGameObject->AddComponent(text);
  this->AddObject(textGameObject);

  //MUSICA
  music.Play(-1);
}

void TitleState::Update(float dt) {
  InputManager &inputManager = InputManager::GetInstance();

  if (inputManager.QuitRequested()) {
    Log::warning("TITLE_STATE - Quit requested via SDL event");
    this->RequestQuit();
  }

  if (inputManager.KeyPress(SPACE_KEY))
  {
    Log::info("TITLE_STATE - Enter key pressed, pushing IntroState");
    Game::GetInstance().Push(new IntroState());
  }

  if (inputManager.KeyPress(M_KEY))
  {
    Log::info("TITLE_STATE - M key pressed, popping TitleState");
    Game::GetInstance().Push(new StageState());
  }

  if (inputManager.KeyPress(N_KEY))
  {
    Log::info("TITLE_STATE - N key pressed, popping TitleState");
    Game::GetInstance().Push(new EndState());
  }

  if (inputManager.KeyPress(B_KEY))
  {
    GameData::playerVictory = true;
    Log::info("TITLE_STATE - B key pressed, popping TitleState");
    Game::GetInstance().Push(new EndState());
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
