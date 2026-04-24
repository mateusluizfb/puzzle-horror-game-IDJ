#include <memory>
#include <algorithm>

#include "DebugState.h"
#include "Log.h"
#include "TileSet.h"
#include "TileMap.h"
#include "InputManager.h"
#include "SpriteRenderer.h"
#include "Character.h"
#include "PlayerController.h"
#include "Collider.h"
#include "Collision.h"
#include "Camera.h"
#include "WaveSpawner.h"
#include "EndState.h"
#include "Game.h"
#include "GameData.h"

static bool AllEnemiesDead(State &state)
{
  auto objectArray = state.GetObjectArray();

  for (const auto &obj : objectArray)
  {
    Zombie* zombie = obj->GetComponent<Zombie>();
    Character *character = obj->GetComponent<Character>();

    if (zombie != nullptr)
    {
      return false;
    }

    if (character != nullptr && character->player == nullptr)
    {
      return false;
    }
  }

  Log::debug("DEBUG_STATE - All enemies dead!");

  return true;
}

static bool AllWavesCompleted(State &state)
{
  auto objectArray = state.GetObjectArray();

  for (const auto &obj : objectArray)
  {
    WaveSpawner *waveSpawner = obj->GetComponent<WaveSpawner>();

    if (waveSpawner != nullptr)
    {
      return waveSpawner->AllWavesCompleted();
    }
  }

  Log::debug("DEBUG_STATE - No WaveSpawner found in state");
  return false;
}

DebugState::DebugState(): State(), music("game/audio/BGM.wav")
{
  Log::info("DEBUG_STATE - Initializing state");

  Camera::GetInstance().SetPosition(600, 450);
  Camera::GetInstance().SetSpeed(200, 200);
}

DebugState::~DebugState()
{
  Log::info("DEBUG_STATE - Destroying state");

  objectArray.clear();
}

void DebugState::Start()
{
  Log::info("DEBUG_STATE - Starting state");

  GameObject *waveSpawnerGameObject = new GameObject();
  waveSpawnerGameObject->AddComponent(new WaveSpawner(*waveSpawnerGameObject));
  this->AddObject(waveSpawnerGameObject);

  for (size_t i = 0; i < objectArray.size(); i++) {
    objectArray[i]->Start();
  }

  started = true;
}

void DebugState::LoadAssets()
{
  Log::debug("DEBUG_STATE - Starting background game object");
  GameObject *bgGameObject = new GameObject();
  bgGameObject->AddComponent(new SpriteRenderer(*bgGameObject, "game/assets/img/Background.png"));
  SpriteRenderer *bgSprite = bgGameObject->GetComponent<SpriteRenderer>();
  bgSprite->SetCameraFollower(true);
  this->AddObject(bgGameObject);
  Log::debug("DEBUG_STATE - Background game object loaded");

  Log::debug("DEBUG_STATE - Starting TileMap game object (TMX)");
  GameObject *tileMapGameObject = new GameObject();
  TileSet *tileSet = new TileSet(64, 64, "game/assets/img/Tileset.png");
  TileMap *tileMap = new TileMap(*tileMapGameObject, "game/assets/map/tilemap.tmx", tileSet);
  tileMapGameObject->AddComponent(tileMap);
  this->AddObject(tileMapGameObject);
  Log::debug("DEBUG_STATE - TileMap game object loaded");

  Log::debug("DEBUG_STATE - Starting Character game object");
  GameObject *characterGameObject = new GameObject();
  Character *character = new Character(*characterGameObject, "game/assets/img/Player.png");
  character->player = character;
  Collider *collider = new Collider(*characterGameObject, Vec2(1, 1), Vec2(1, 1));
  PlayerController *playerController = new PlayerController(*characterGameObject);
  characterGameObject->AddComponent(character);
  characterGameObject->AddComponent(collider);
  characterGameObject->AddComponent(playerController);
  characterGameObject->tag = "player";
  this->AddObject(characterGameObject);
  SpriteRenderer *spriteRenderer1 = characterGameObject->GetComponent<SpriteRenderer>();
  spriteRenderer1->SetPosition(1253, 901);
  Log::debug("DEBUG_STATE - Character game object loaded");

  Camera::GetInstance().Follow(this->GetObjectPtr(characterGameObject).lock().get());
}

void DebugState::Update(float dt)
{
  InputManager& inputManager = InputManager::GetInstance();

  std::weak_ptr<GameObject> playerPtr = this->GetObjectByTag("player");
  if (playerPtr.expired())
  {
    Log::info("DEBUG_STATE - Player is dead, switching to EndState");
    music.Stop();
    popRequested = true;
    GameData::playerVictory = false;
    Game::GetInstance().Push(new EndState());
  }

  if (AllWavesCompleted(*this) && AllEnemiesDead(*this))
  {
    Log::info("DEBUG_STATE - All waves completed and all enemies dead, switching to EndState");
    music.Stop();
    popRequested = true;
    GameData::playerVictory = true;
    Game::GetInstance().Push(new EndState());
  }

  if (inputManager.QuitRequested())
  {
    Log::warning("DEBUG_STATE - Quit requested via SDL event");
    music.Stop();
    this->RequestQuit();
  }

  if (inputManager.KeyPress(ESCAPE_KEY))
  {
    Log::info("DEBUG_STATE - Escape key pressed, popping state");
    music.Stop();
    this->RequestPop();
  }

  for (size_t i = 0; i < objectArray.size(); i++)
  {
    objectArray[i]->Update(dt);
  }

  collisionSystem.Update(objectArray);

  for (size_t i = 0; i < objectArray.size(); i++)
  {
    if (objectArray[i]->IsDead()) {
      Log::info("DEBUG_STATE - Removing dead game object");
      objectArray.erase(objectArray.begin() + i);
    }
  }

  Camera::GetInstance().Update(dt);
}

void DebugState::Render()
{
  RenderArray();
}

void DebugState::Pause()
{
  Log::info("DEBUG_STATE - Pausing state");
}

void DebugState::Resume()
{
  Log::info("DEBUG_STATE - Resuming state");
}
