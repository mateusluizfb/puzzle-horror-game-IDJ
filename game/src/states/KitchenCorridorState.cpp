#include "State.h"
#include "KitchenCorridorState.h"
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
#include "TileObject.h"
#include "Pushable.h"
#include "Wall.h"
#include "StagePush.h"
#include "Quiz.h"
#include "QuizUI.h"
#include "KitchenState.h"

KitchenCorridorState::KitchenCorridorState(): State(), music("game/audio/BGM.wav")
{
  Log::info("KITCHENCORRIDOR_STATE - Initializing state");

  Camera::GetInstance().SetPosition(600, 450);
  Camera::GetInstance().SetSpeed(200, 200);
}

KitchenCorridorState::~KitchenCorridorState()
{
  Log::info("KITCHENCORRIDOR_STATE - Destroying state");

  objectArray.clear();
}

void KitchenCorridorState::Start()
{
  Log::info("KITCHENCORRIDOR_STATE - Starting state");

  StartArray();
}

void KitchenCorridorState::LoadAssets()
{
  Vec2 tileScale = GameData::tileScale;
  Camera::GetInstance().SetPosition(0, 0);

  Log::debug("KITCHENCORRIDOR_STATE - Starting background game object");
  GameObject *bgGameObject = new GameObject();
  bgGameObject->AddComponent(new SpriteRenderer(*bgGameObject, "game/assets/img/black_solid_color.png"));
  SpriteRenderer *bgSprite = bgGameObject->GetComponent<SpriteRenderer>();
  bgSprite->SetCameraFollower(true);
  this->AddObject(bgGameObject);
  Log::debug("KITCHENCORRIDOR_STATE - Background game object loaded");

  Log::debug("KITCHENCORRIDOR_STATE - Starting TileMap game object (TMX)");
  GameObject *tileMapGameObject = new GameObject();
  TileSet *tileSet = new TileSet(16, 16, "game/assets/tiles/test_tileset.png");
  TileMap *tileMap = new TileMap(*tileMapGameObject, "game/assets/tiles/kitchen_corridor.tmx", tileSet);
  tileMap->scale = tileScale;
  tileMapGameObject->AddComponent(tileMap);
  this->AddObject(tileMapGameObject);
  Log::debug("KITCHENCORRIDOR_STATE - TileMap game object loaded");

  Log::debug("KITCHENCORRIDOR_STATE - Starting Character game object");
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
  spriteRenderer1->SetPosition(535, 768);
  Log::debug("KITCHENCORRIDOR_STATE - Character game object loaded");

  Log::debug("KITCHENCORRIDOR_STATE - Starting TileObjects loader");
  TileObjects tileObjects(
      "game/assets/tiles/kitchen_corridor.tmx",
      "game/assets/tiles/test_tileset.png",
      tileScale
  );
  tileObjects.RegisterComponent("pushable", [](GameObject& go) -> Component* {
    return new Pushable(go, 200.0f);
  });
  tileObjects.RegisterComponent("wall", [](GameObject& go) -> Component* {
    return new Wall(go);
  });
  tileObjects.RegisterComponent("stage_push", [](GameObject &go) -> Component *
                                 {
    const TileObjectData& data = go.GetComponent<TileObject>()->GetData();
    std::string stageName = "TitleState";
    auto it = data.properties.find("stage_change_name");
    if (it != data.properties.end()) {
      stageName = it->second;
    }
    return new StagePush(go, stageName);
  });
  tileObjects.RegisterComponent("collider", [](GameObject& go) -> Component* {
    return new Collider(go, Vec2(1, 1), Vec2(0, 0));
  });
  tileObjects.RegisterComponent("composite_collider", [](GameObject& go) -> Component* {
    return new Collider(go, Vec2(1, 1), Vec2(0, 0));
  });
  
  tileObjects.Load(*this);

  Log::debug("KITCHENCORRIDOR_STATE - TileObjects loader finished");
}

void KitchenCorridorState::Update(float dt)
{
  InputManager& inputManager = InputManager::GetInstance();

  std::weak_ptr<GameObject> playerPtr = this->GetObjectByTag("player");
  if (playerPtr.expired())
  {
    Log::info("KITCHENCORRIDOR_STATE - Player is dead, switching to EndState");
    music.Stop();
    popRequested = true;
    GameData::playerVictory = false;
    Game::GetInstance().Push(new EndState());
  }

  if (inputManager.QuitRequested())
  {
    Log::warning("KITCHENCORRIDOR_STATE - Quit requested via SDL event");
    music.Stop();
    this->RequestQuit();
  }

  if (inputManager.KeyPress(ESCAPE_KEY))
  {
    Log::info("KITCHENCORRIDOR_STATE - Escape key pressed, popping state");
    music.Stop();
    this->RequestPop();
  }

  if (inputManager.KeyPress(Z_KEY))
  {
    Log::info("KITCHENCORRIDOR_STATE - Z key pressed, popping state");
    music.Stop();
    this->RequestPop();
  }

  if (inputManager.KeyPress(X_KEY))
  {
    Log::info("KITCHENCORRIDOR_STATE - X key pressed, pushing KitchenState");
    music.Stop();
    Game::GetInstance().Push(new KitchenState());
  }

  UpdateArray(dt);

  collisionSystem.Update(objectArray);

  UpdateDead();

  Camera::GetInstance().Update(dt);
}

void KitchenCorridorState::Render()
{
  RenderArray();
}

void KitchenCorridorState::Pause()
{
  Log::info("KITCHENCORRIDOR_STATE - Pausing state");
}

void KitchenCorridorState::Resume()
{
  Log::info("KITCHENCORRIDOR_STATE - Resuming state");
  GameObject* player = this->GetObjectByTag("player").lock().get();
  
  if (!player) {
    Log::error("KITCHENCORRIDOR_STATE - Cannot resume: player object not found");
    return;
  }

  Camera::GetInstance().Follow(player);
}
