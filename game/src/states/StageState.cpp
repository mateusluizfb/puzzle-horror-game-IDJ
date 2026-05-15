#include "State.h"
#include "StageState.h"
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
#include "EndState.h"
#include "Game.h"
#include "GameData.h"
#include "GameObject.h"
#include "TileMap.h"
#include "TileObjects.h"
#include "Pushable.h"

StageState::StageState(): State(), music("game/audio/BGM.wav")
{
  Log::info("STAGE_STATE - Initializing state");

  Camera::GetInstance().SetPosition(600, 450);
  Camera::GetInstance().SetSpeed(200, 200);
}

StageState::~StageState()
{
  Log::info("STAGE_STATE - Destroying state");

  objectArray.clear();
}

void StageState::Start()
{
  Log::info("STAGE_STATE - Starting state");

  for (size_t i = 0; i < objectArray.size(); i++) {
    objectArray[i]->Start();
  }

  started = true;
}

void StageState::LoadAssets()
{
  Vec2 tileScale = GameData::tileScale;

  Log::debug("STAGE_STATE - Starting background game object");
  GameObject *bgGameObject = new GameObject();
  bgGameObject->AddComponent(new SpriteRenderer(*bgGameObject, "game/assets/img/Background.png"));
  SpriteRenderer *bgSprite = bgGameObject->GetComponent<SpriteRenderer>();
  bgSprite->SetCameraFollower(true);
  this->AddObject(bgGameObject);
  Log::debug("STAGE_STATE - Background game object loaded");

  Log::debug("STAGE_STATE - Starting TileMap game object (TMX)");
  GameObject *tileMapGameObject = new GameObject();
  TileSet *tileSet = new TileSet(16, 16, "game/assets/tiles/tileset.png");
  TileMap *tileMap = new TileMap(*tileMapGameObject, "game/assets/tiles/map.tmx", tileSet);
  tileMap->scale = tileScale;
  tileMapGameObject->AddComponent(tileMap);
  this->AddObject(tileMapGameObject);
  Log::debug("STAGE_STATE - TileMap game object loaded");

  Log::debug("STAGE_STATE - Starting Character game object");
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
  spriteRenderer1->SetPosition(584, 712);
  Log::debug("STAGE_STATE - Character game object loaded");

  Log::debug("STAGE_STATE - Starting TileObjects loader");
  TileObjects tileObjects(
      "game/assets/tiles/map.tmx",
      "game/assets/tiles/tileset.png",
      tileScale
  );
  tileObjects.RegisterComponent("pushable", [](GameObject& go) -> Component* {
    return new Pushable(go, 100.0f);
  });
  tileObjects.RegisterComponent("collider", [](GameObject& go) -> Component* {
    return new Collider(go, Vec2(1, 1), Vec2(0, 0));
  });
  tileObjects.Load(*this);
  Log::debug("STAGE_STATE - TileObjects loader finished");

  Camera::GetInstance().Follow(this->GetObjectPtr(characterGameObject).lock().get());
}

void StageState::Update(float dt)
{
  InputManager& inputManager = InputManager::GetInstance();

  std::weak_ptr<GameObject> playerPtr = this->GetObjectByTag("player");
  if (playerPtr.expired())
  {
    Log::info("STAGE_STATE - Player is dead, switching to EndState");
    music.Stop();
    popRequested = true;
    GameData::playerVictory = false;
    Game::GetInstance().Push(new EndState());
  }

  if (inputManager.QuitRequested())
  {
    Log::warning("STAGE_STATE - Quit requested via SDL event");
    music.Stop();
    this->RequestQuit();
  }

  if (inputManager.KeyPress(ESCAPE_KEY))
  {
    Log::info("STAGE_STATE - Escape key pressed, popping state");
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
      Log::info("STAGE_STATE - Removing dead game object");
      objectArray.erase(objectArray.begin() + i);
    }
  }

  Camera::GetInstance().Update(dt);
}

void StageState::Render()
{
  RenderArray();
}

void StageState::Pause()
{
  Log::info("STAGE_STATE - Pausing state");
}

void StageState::Resume()
{
  Log::info("STAGE_STATE - Resuming state");
}
