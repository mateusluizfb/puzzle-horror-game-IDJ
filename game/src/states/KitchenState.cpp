#include "State.h"
#include "KitchenState.h"
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
#include "BedroomState.h"

KitchenState::KitchenState(): State(), music("game/audio/BGM.wav")
{
  Log::info("KITCHEN_STATE - Initializing state");

  Camera::GetInstance().SetPosition(600, 450);
  Camera::GetInstance().SetSpeed(200, 200);
}

KitchenState::~KitchenState()
{
  Log::info("KITCHEN_STATE - Destroying state");

  objectArray.clear();
}

void KitchenState::Start()
{
  Log::info("KITCHEN_STATE - Starting state");

  StartArray();
}

void KitchenState::LoadAssets()
{
  Vec2 tileScale = GameData::tileScale;

  Log::debug("KITCHEN_STATE - Starting background game object");
  GameObject *bgGameObject = new GameObject();
  bgGameObject->AddComponent(new SpriteRenderer(*bgGameObject, "game/assets/img/Background.png"));
  SpriteRenderer *bgSprite = bgGameObject->GetComponent<SpriteRenderer>();
  bgSprite->SetCameraFollower(true);
  this->AddObject(bgGameObject);
  Log::debug("KITCHEN_STATE - Background game object loaded");

  Log::debug("KITCHEN_STATE - Starting TileMap game object (TMX)");
  GameObject *tileMapGameObject = new GameObject();
  TileSet *tileSet = new TileSet(16, 16, "game/assets/tiles/tileset.png");
  TileMap *tileMap = new TileMap(*tileMapGameObject, "game/assets/tiles/map.tmx", tileSet);
  tileMap->scale = tileScale;
  tileMapGameObject->AddComponent(tileMap);
  this->AddObject(tileMapGameObject);
  Log::debug("KITCHEN_STATE - TileMap game object loaded");

  Log::debug("KITCHEN_STATE - Starting Character game object");
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
  Log::debug("KITCHEN_STATE - Character game object loaded");

  Log::debug("KITCHEN_STATE - Starting TileObjects loader");
  TileObjects tileObjects(
      "game/assets/tiles/map.tmx",
      "game/assets/tiles/tileset.png",
      tileScale
  );
  tileObjects.RegisterComponent("pushable", [](GameObject& go) -> Component* {
    return new Pushable(go, 100.0f);
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

  Log::debug("KITCHEN_STATE - TileObjects loader finished");

  Camera::GetInstance().Follow(this->GetObjectPtr(characterGameObject).lock().get());
}

void KitchenState::Update(float dt)
{
  InputManager& inputManager = InputManager::GetInstance();

  std::weak_ptr<GameObject> playerPtr = this->GetObjectByTag("player");
  if (playerPtr.expired())
  {
    Log::info("KITCHEN_STATE - Player is dead, switching to EndState");
    music.Stop();
    popRequested = true;
    GameData::playerVictory = false;
    Game::GetInstance().Push(new EndState());
  }

  if (inputManager.QuitRequested())
  {
    Log::warning("KITCHEN_STATE - Quit requested via SDL event");
    music.Stop();
    this->RequestQuit();
  }

  if (inputManager.KeyPress(ESCAPE_KEY))
  {
    Log::info("KITCHEN_STATE - Escape key pressed, popping state");
    music.Stop();
    this->RequestPop();
  }

  if (inputManager.KeyPress(Z_KEY))
  {
    Log::info("KITCHEN_STATE - Z key pressed, popping state");
    music.Stop();
    this->RequestPop();
  }

  if (inputManager.KeyPress(X_KEY))
  {
    Log::info("KITCHEN_STATE - X key pressed, pushing BedroomState");
    music.Stop();
    Game::GetInstance().Push(new BedroomState());
  }

  UpdateArray(dt);

  collisionSystem.Update(objectArray);

  UpdateDead();

  Camera::GetInstance().Update(dt);
}

void KitchenState::Render()
{
  RenderArray();
}

void KitchenState::Pause()
{
  Log::info("KITCHEN_STATE - Pausing state");
}

void KitchenState::Resume()
{
  Log::info("KITCHEN_STATE - Resuming state");
  GameObject* player = this->GetObjectByTag("player").lock().get();
  
  if (!player) {
    Log::error("KITCHEN_STATE - Cannot resume: player object not found");
    return;
  }

  Camera::GetInstance().Follow(player);
}
