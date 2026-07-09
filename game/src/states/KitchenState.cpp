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
#include "Document.h"
#include "Rat.h"

KitchenState::KitchenState(): State(), music("game/assets/music/soundtrack.wav")
{
  Log::info("KITCHEN_STATE - Initializing state");

  Camera::GetInstance().SetPosition(600, 450);
  Camera::GetInstance().SetSpeed(200, 200);
}

KitchenState::~KitchenState()
{
  Log::info("KITCHEN_STATE - Destroying state");

  objectArray.clear();
  //music.Stop();
}

void KitchenState::Start()
{
  Log::info("KITCHEN_STATE - Starting state");

  StartArray();

  //music.Play(-1);
}

void KitchenState::LoadAssets()
{
  Vec2 tileScale = GameData::tileScaleClose;
  Camera::GetInstance().SetPosition(0, 0);

  Log::debug("KITCHEN_STATE - Starting background game object");
  GameObject *bgGameObject = new GameObject();
  bgGameObject->AddComponent(new SpriteRenderer(*bgGameObject, "game/assets/img/black_solid_color.png"));
  SpriteRenderer *bgSprite = bgGameObject->GetComponent<SpriteRenderer>();
  bgSprite->SetCameraFollower(true);
  this->AddObject(bgGameObject);
  Log::debug("KITCHEN_STATE - Background game object loaded");

  Log::debug("KITCHEN_STATE - Starting TileMap game object (TMX)");
  GameObject *tileMapGameObject = new GameObject();
  TileSet *tileSet = new TileSet(16, 16, "game/assets/tiles/test_tileset.png");
  TileMap *tileMap = new TileMap(*tileMapGameObject, "game/assets/tiles/kitchen.tmx", tileSet);
  tileMap->scale = tileScale;
  tileMapGameObject->AddComponent(tileMap);
  this->AddObject(tileMapGameObject);
  Log::debug("KITCHEN_STATE - TileMap game object loaded");

  Log::debug("KITCHEN_STATE - Starting Character game object");
  GameObject *characterGameObject = new GameObject();
  Character *character = new Character(*characterGameObject, "game/assets/img/Player.png");
  character->player = character;
  PlayerController *playerController = new PlayerController(*characterGameObject);
  characterGameObject->AddComponent(character);
  characterGameObject->AddComponent(playerController);
  characterGameObject->tag = "player";
  this->AddObject(characterGameObject);
  SpriteRenderer *spriteRenderer1 = characterGameObject->GetComponent<SpriteRenderer>();
  spriteRenderer1->SetPosition(925, 430);
  Log::debug("KITCHEN_STATE - Character game object loaded");

  Log::debug("KITCHEN_STATE - Starting TileObjects loader");
  TileObjects tileObjects(
      "game/assets/tiles/kitchen.tmx",
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
    DialogueBox::PortraitMode mode = DialogueBox::PortraitMode::STILL;
    auto modeIt = data.properties.find("document_portrait_mode");
    if (modeIt != data.properties.end() && modeIt->second == "thinking") {
      mode = DialogueBox::PortraitMode::THINKING;
    }
    return new Document(go, text, mode);
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

  Log::debug("KITCHEN_STATE - TileObjects loader finished");

  Log::debug("KITCHEN_STATE - Starting Rat game object");
  GameObject *ratGameObject = new GameObject();
  Rat *rat = new Rat(*ratGameObject, 700, 205);
  ratGameObject->AddComponent(rat);
  this->AddObject(ratGameObject);
  Log::debug("KITCHEN_STATE - Rat game object loaded");
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
    //music.Stop();
    this->RequestQuit();
  }

  if (inputManager.KeyPress(ESCAPE_KEY))
  {
    Log::info("KITCHEN_STATE - Escape key pressed, quitting game");
    //music.Stop();
    this->RequestQuit();
  }

  if (inputManager.KeyPress(Z_KEY))
  {
    Log::info("KITCHEN_STATE - Z key pressed, popping state");
    //music.Stop();
    this->RequestPop();
  }

  if (inputManager.KeyPress(X_KEY))
  {
    Log::info("KITCHEN_STATE - X key pressed, pushing BedroomState");
    //music.Stop();
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
  //music.Play(-1);
}
