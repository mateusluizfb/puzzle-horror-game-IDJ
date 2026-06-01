#include "DebugState.h"
#include "GameObject.h"
#include "SpriteRenderer.h"
#include "Log.h"
#include "InputManager.h"
#include "Game.h"
#include "StageState.h"
#include "StageState.h"
#include "Camera.h"
#include "Text.h"
#include "TileSet.h"
#include "TileMap.h"

DebugState::DebugState() : State()
{
  Log::info("DEBUG_STATE - Initializing DebugState");
}

DebugState::~DebugState()
{
  Log::info("DEBUG_STATE - Destroying DebugState");

  objectArray.clear();
}

void DebugState::LoadAssets()
{
  Log::info("DEBUG_STATE - Loading assets for DebugState");

  GameObject *titleGameObject = new GameObject();
  titleGameObject->AddComponent(new SpriteRenderer(*titleGameObject, "game/assets/img/black_solid_color.png"));
  this->AddObject(titleGameObject);

  Log::debug("DEBUG_STATE - Starting TileMap game object (TMX)");
  GameObject *tileMapGameObject = new GameObject();
  TileSet *tileSet = new TileSet(16, 16, "game/assets/tiles/kitchen_tileset.png");
  TileMap *tileMap = new TileMap(*tileMapGameObject, "game/assets/tiles/debug.tmx", tileSet);
  // tileMap->scale = Vec2(2.0f, 2.0f);
  tileMapGameObject->AddComponent(tileMap);
  this->AddObject(tileMapGameObject);
  Log::debug("DEBUG_STATE - TileMap game object loaded");
}

void DebugState::Update(float dt)
{
  InputManager &inputManager = InputManager::GetInstance();

  if (inputManager.KeyPress(ESCAPE_KEY))
  {
    Log::info("DEBUG_STATE - Escape key pressed, popping state");
    this->RequestPop();
  }

  if (inputManager.QuitRequested())
  {
    Log::warning("DEBUG_STATE - Quit requested via SDL event");
    this->RequestQuit();
  }

  if (inputManager.IsKeyDown(LEFT_ARROW_KEY))
  {
    cameraPosition.x -= 200 * dt;
  }

  if (inputManager.IsKeyDown(RIGHT_ARROW_KEY))
  {
    cameraPosition.x += 200 * dt;
  }

  if (inputManager.IsKeyDown(UP_ARROW_KEY))
  {
    cameraPosition.y -= 200 * dt;
  }

  if (inputManager.IsKeyDown(DOWN_ARROW_KEY))
  {
    cameraPosition.y += 200 * dt;
  }

  Camera::GetInstance().SetPosition(cameraPosition.x, cameraPosition.y);

  UpdateArray(dt);
}

void DebugState::Render()
{
  RenderArray();
}

void DebugState::Resume()
{
  Camera::GetInstance().SetPosition(0, 0);
}

void DebugState::Pause() {}
void DebugState::Start() {}
