#include "State.h"
#include "LivingRoomCorridorState.h"
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
#include "Document.h"
#include "LivingRoomState.h"

LivingRoomCorridorState::LivingRoomCorridorState(): State(), music("game/assets/music/soundtrack.wav")
{
  Log::info("LIVINGROOMCORRIDOR_STATE - Initializing state");

  Camera::GetInstance().SetPosition(0, 0);
}

LivingRoomCorridorState::~LivingRoomCorridorState()
{
  Log::info("LIVINGROOMCORRIDOR_STATE - Destroying state");

  objectArray.clear();
}

void LivingRoomCorridorState::Start()
{
  Log::info("LIVINGROOMCORRIDOR_STATE - Starting state");

  StartArray();

  //music.Play(-1);
}

void LivingRoomCorridorState::LoadAssets()
{
  Vec2 tileScale = GameData::tileScaleClose;

  Log::debug("LIVINGROOMCORRIDOR_STATE - Starting background game object");
  GameObject *bgGameObject = new GameObject();
  bgGameObject->AddComponent(new SpriteRenderer(*bgGameObject, "game/assets/img/black_solid_color.png"));
  SpriteRenderer *bgSprite = bgGameObject->GetComponent<SpriteRenderer>();
  bgSprite->SetCameraFollower(true);
  this->AddObject(bgGameObject);
  Log::debug("LIVINGROOMCORRIDOR_STATE - Background game object loaded");

  Log::debug("LIVINGROOMCORRIDOR_STATE - Starting TileMap game object (TMX)");
  GameObject *tileMapGameObject = new GameObject();
  TileSet *tileSet = new TileSet(16, 16, "game/assets/tiles/test_tileset.png");
  TileMap *tileMap = new TileMap(*tileMapGameObject, "game/assets/tiles/living_room_corridor.tmx", tileSet);
  tileMap->scale = tileScale;
  tileMapGameObject->AddComponent(tileMap);
  this->AddObject(tileMapGameObject);
  Log::debug("LIVINGROOMCORRIDOR_STATE - TileMap game object loaded");

  Log::debug("LIVINGROOMCORRIDOR_STATE - Starting Character game object");
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
  spriteRenderer1->SetPosition(65, 550);
  Log::debug("LIVINGROOMCORRIDOR_STATE - Character game object loaded");

  Log::debug("LIVINGROOMCORRIDOR_STATE - Starting TileObjects loader");
  TileObjects tileObjects(
      "game/assets/tiles/living_room_corridor.tmx",
      "game/assets/tiles/test_tileset.png",
      tileScale
  );
  tileObjects.RegisterComponent("document", [](GameObject &go) -> Component * {
    const TileObjectData& data = go.GetComponent<TileObject>()->GetData();
    std::string text;
    auto it = data.properties.find("document_text");
    if (it != data.properties.end()) {
      text = it->second;
    }
    return new Document(go, text);
  });
  tileObjects.RegisterComponent("pushable", [](GameObject& go) -> Component* {
    return new Pushable(go, 150.0f);
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

  tileObjects.Load(*this);

  Log::debug("LIVINGROOMCORRIDOR_STATE - TileObjects loader finished");

}

void LivingRoomCorridorState::Update(float dt)
{
  InputManager& inputManager = InputManager::GetInstance();

  std::weak_ptr<GameObject> playerPtr = this->GetObjectByTag("player");
  if (playerPtr.expired())
  {
    Log::info("LIVINGROOMCORRIDOR_STATE - Player is dead, switching to EndState");
    music.Stop();
    popRequested = true;
    GameData::playerVictory = false;
    Game::GetInstance().Push(new EndState());
  }

  if (inputManager.QuitRequested())
  {
    Log::warning("LIVINGROOMCORRIDOR_STATE - Quit requested via SDL event");
    //music.Stop();
    this->RequestQuit();
  }

  if (inputManager.KeyPress(ESCAPE_KEY))
  {
    Log::info("LIVINGROOMCORRIDOR_STATE - Escape key pressed, popping state");
    //music.Stop();
    this->RequestPop();
  }

  if (inputManager.KeyPress(Z_KEY))
  {
    Log::info("LIVINGROOMCORRIDOR_STATE - Z key pressed, popping state");
    //music.Stop();
    this->RequestPop();
  }

  if (inputManager.KeyPress(X_KEY))
  {
    Log::info("LIVINGROOMCORRIDOR_STATE - X key pressed, pushing LivingRoomState");
    //music.Stop();
    Game::GetInstance().Push(new LivingRoomState());
  }

  UpdateArray(dt);

  collisionSystem.Update(objectArray);

  UpdateDead();

  Camera::GetInstance().Update(dt);
}

void LivingRoomCorridorState::Render()
{
  RenderArray();
}

void LivingRoomCorridorState::Pause()
{
  Log::info("LIVINGROOMCORRIDOR_STATE - Pausing state");
}

void LivingRoomCorridorState::Resume()
{
  Log::info("LIVINGROOMCORRIDOR_STATE - Resuming state");
  GameObject* player = this->GetObjectByTag("player").lock().get();
  
  if (!player) {
    Log::error("LIVINGROOMCORRIDOR_STATE - Cannot resume: player object not found");
    return;
  }
  music.Play(-1);
}
