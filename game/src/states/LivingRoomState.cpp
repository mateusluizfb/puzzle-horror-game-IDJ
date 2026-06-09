#include "State.h"
#include "LivingRoomState.h"
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
#include "KitchenCorridorState.h"

LivingRoomState::LivingRoomState(): State(), music("game/audio/BGM.wav")
{
  Log::info("LIVINGROOM_STATE - Initializing state");

  Camera::GetInstance().SetPosition(600, 450);
  Camera::GetInstance().SetSpeed(200, 200);
}

LivingRoomState::~LivingRoomState()
{
  Log::info("LIVINGROOM_STATE - Destroying state");

  objectArray.clear();
}

void LivingRoomState::Start()
{
  Log::info("LIVINGROOM_STATE - Starting state");

  StartArray();
}

void LivingRoomState::LoadAssets()
{
  Vec2 tileScale = GameData::tileScaleFar;
  Camera::GetInstance().SetPosition(0, 0);

  Log::debug("LIVINGROOM_STATE - Starting background game object");
  GameObject *bgGameObject = new GameObject();
  bgGameObject->AddComponent(new SpriteRenderer(*bgGameObject, "game/assets/img/black_solid_color.png"));
  SpriteRenderer *bgSprite = bgGameObject->GetComponent<SpriteRenderer>();
  bgSprite->SetCameraFollower(true);
  this->AddObject(bgGameObject);
  Log::debug("LIVINGROOM_STATE - Background game object loaded");

  Log::debug("LIVINGROOM_STATE - Starting TileMap game object (TMX)");
  GameObject *tileMapGameObject = new GameObject();
  TileSet *tileSet = new TileSet(16, 16, "game/assets/tiles/test_tileset.png");
  TileMap *tileMap = new TileMap(*tileMapGameObject, "game/assets/tiles/living_room.tmx", tileSet);
  tileMap->scale = tileScale;
  tileMapGameObject->AddComponent(tileMap);
  this->AddObject(tileMapGameObject);
  Log::debug("LIVINGROOM_STATE - TileMap game object loaded");

  Log::debug("LIVINGROOM_STATE - Starting Character game object");
  GameObject *characterGameObject = new GameObject();
  Character *character = new Character(*characterGameObject, "game/assets/img/Player_Small.png");
  character->player = character;
  Collider *collider = new Collider(*characterGameObject, Vec2(1, 1), Vec2(1, 1));
  PlayerController *playerController = new PlayerController(*characterGameObject);
  characterGameObject->AddComponent(character);
  characterGameObject->AddComponent(collider);
  characterGameObject->AddComponent(playerController);
  characterGameObject->tag = "player";
  this->AddObject(characterGameObject);
  SpriteRenderer *spriteRenderer1 = characterGameObject->GetComponent<SpriteRenderer>();
  spriteRenderer1->SetPosition(741, 161);
  Log::debug("LIVINGROOM_STATE - Character game object loaded");

  Log::debug("LIVINGROOM_STATE - Starting TileObjects loader");
  TileObjects tileObjects(
      "game/assets/tiles/living_room.tmx",
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

  Log::debug("LIVINGROOM_STATE - TileObjects loader finished");
}

void LivingRoomState::Update(float dt)
{
  InputManager& inputManager = InputManager::GetInstance();

  std::weak_ptr<GameObject> playerPtr = this->GetObjectByTag("player");
  if (playerPtr.expired())
  {
    Log::info("LIVINGROOM_STATE - Player is dead, switching to EndState");
    music.Stop();
    popRequested = true;
    GameData::playerVictory = false;
    Game::GetInstance().Push(new EndState());
  }

  if (inputManager.QuitRequested())
  {
    Log::warning("LIVINGROOM_STATE - Quit requested via SDL event");
    music.Stop();
    this->RequestQuit();
  }

  if (inputManager.KeyPress(ESCAPE_KEY))
  {
    Log::info("LIVINGROOM_STATE - Escape key pressed, popping state");
    music.Stop();
    this->RequestPop();
  }

  if (inputManager.KeyPress(Z_KEY))
  {
    Log::info("LIVINGROOM_STATE - Z key pressed, popping state");
    music.Stop();
    this->RequestPop();
  }

  if (inputManager.KeyPress(X_KEY))
  {
    Log::info("LIVINGROOM_STATE - X key pressed, pushing KitchenCorridorState");
    music.Stop();
    Game::GetInstance().Push(new KitchenCorridorState());
  }

  UpdateArray(dt);

  collisionSystem.Update(objectArray);

  UpdateDead();

  Camera::GetInstance().Update(dt);
}

void LivingRoomState::Render()
{
  RenderArray();
}

void LivingRoomState::Pause()
{
  Log::info("LIVINGROOM_STATE - Pausing state");
}

void LivingRoomState::Resume()
{
  Log::info("LIVINGROOM_STATE - Resuming state");
  GameObject* player = this->GetObjectByTag("player").lock().get();
  
  if (!player) {
    Log::error("LIVINGROOM_STATE - Cannot resume: player object not found");
    return;
  }

  Camera::GetInstance().Follow(player);
}
