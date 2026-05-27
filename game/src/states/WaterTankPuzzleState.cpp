#include "Game.h"
#include "WaterTankPuzzleState.h"
#include "TitleState.h"
#include "GameData.h"
#include "SpriteRenderer.h"
#include "Log.h"
#include "InputManager.h"
#include "Camera.h"
#include "TileMap.h"
#include "TileSet.h"

WaterTankPuzzleState::WaterTankPuzzleState() : State(), backgroundMusic()
{
  Log::info("PIPE_PUZZLE_STATE - Initializing WaterTankPuzzleState");
}

WaterTankPuzzleState::~WaterTankPuzzleState()
{
  Log::info("PIPE_PUZZLE_STATE - Destroying WaterTankPuzzleState");

  objectArray.clear();
}

void WaterTankPuzzleState::LoadAssets()
{
  Log::info("PIPE_PUZZLE_STATE - Loading assets for WaterTankPuzzleState");

  Log::debug("PIPE_PUZZLE_STATE - Starting background game object");
  GameObject *bgGameObject = new GameObject();
  bgGameObject->AddComponent(new SpriteRenderer(*bgGameObject, "game/assets/img/black_solid_color.png"));
  SpriteRenderer *bgSprite = bgGameObject->GetComponent<SpriteRenderer>();
  bgSprite->SetCameraFollower(true);
  this->AddObject(bgGameObject);
  Log::debug("PIPE_PUZZLE_STATE - Background game object loaded");

  Log::debug("PIPE_PUZZLE_STATE - Starting TileMap game object (TMX)");
  GameObject *tileMapGameObject = new GameObject();
  TileSet *tileSet = new TileSet(32, 32, "game/assets/tiles/pipes_tileset.png");
  TileMap *tileMap = new TileMap(*tileMapGameObject, "game/assets/tiles/water_tank_puzzle.tmx", tileSet);
  // tileMap->scale = tileScale;
  tileMapGameObject->AddComponent(tileMap);
  this->AddObject(tileMapGameObject);
  Log::debug("PIPE_PUZZLE_STATE - TileMap game object loaded");
}

void WaterTankPuzzleState::Update(float dt)
{
  InputManager &inputManager = InputManager::GetInstance();

  if (inputManager.QuitRequested() || inputManager.KeyPress(ESCAPE_KEY))
  {
    Log::warning("PIPE_PUZZLE_STATE - Quit requested via SDL event");
    this->RequestQuit();
  }

  if (inputManager.KeyPress(SPACE_KEY))
  {
    Log::info("PIPE_PUZZLE_STATE - Space key pressed, restarting game");
    popRequested = true;
  }

  UpdateArray(dt);
}

void WaterTankPuzzleState::Render()
{
  RenderArray();
}

void WaterTankPuzzleState::Start() {
  Camera::GetInstance().Unfollow();
  Camera::GetInstance().SetPosition(0, 0);
}
void WaterTankPuzzleState::Pause() {}
void WaterTankPuzzleState::Resume() {}
