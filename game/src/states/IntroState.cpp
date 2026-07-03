#include "IntroState.h"

IntroState::IntroState() : State(), transitioned(false)
{
  Log::info("INTRO_STATE - Initializing state");

  Camera::GetInstance().SetPosition(0, 0);
  LoadAssets();
}

IntroState::~IntroState()
{
  Log::info("INTRO_STATE - Destroying state");

  objectArray.clear();
}

void IntroState::LoadAssets()
{
  Log::debug("INTRO_STATE - Starting background game object");
  GameObject *bgGameObject = new GameObject();
  bgGameObject->AddComponent(new SpriteRenderer(*bgGameObject, "game/assets/img/black_solid_color.png"));
  SpriteRenderer *bgSprite = bgGameObject->GetComponent<SpriteRenderer>();
  bgSprite->SetCameraFollower(true);
  this->AddObject(bgGameObject);
  Log::debug("INTRO_STATE - Background game object loaded");

  Log::debug("INTRO_STATE - Starting dialogue box game object");
  Rect box = Rect(
      0,
      Game::GetInstance().GetWindowHeight() / 1.5,
      Game::GetInstance().GetWindowWidth(),
      Game::GetInstance().GetWindowHeight() / 4);
  SDL_Color white = {255, 255, 255, 255};
  GameObject *dialogueGameObject = CreateDialogueBox(box, "game/assets/font/neodgm.ttf", 32, white, "Nossa que sede... Mas que sensaçããã$1@o estranha... o que foi isso??", {}, DialogueBox::PortraitMode::STILL);
  this->AddObject(dialogueGameObject);
  Log::debug("INTRO_STATE - Dialogue box game object loaded");
}

void IntroState::Start()
{
  Log::info("INTRO_STATE - Starting state");

  StartArray();
}

void IntroState::Update(float dt)
{
  InputManager &inputManager = InputManager::GetInstance();

  if (inputManager.QuitRequested())
  {
    Log::warning("INTRO_STATE - Quit requested via SDL event");
    this->RequestQuit();
  }

  if (inputManager.KeyPress(ESCAPE_KEY))
  {
    Log::info("INTRO_STATE - Escape key pressed, popping state");
    this->RequestPop();
  }

  if (!transitioned)
  {
    std::weak_ptr<GameObject> dialoguePtr = this->GetObjectByTag("dialogue");
    if (!dialoguePtr.expired())
    {
      DialogueBox *db = dialoguePtr.lock()->GetComponent<DialogueBox>();
      if (db && db->IsFinished())
      {
        Log::info("INTRO_STATE - Dialogue finished, pushing BedroomState");
        transitioned = true;
        Game::GetInstance().Push(new BedroomState());
      }
    }
  }

  UpdateArray(dt);
}

void IntroState::Render()
{
  RenderArray();
}

void IntroState::Pause()
{
  Log::info("INTRO_STATE - Pausing state");
}

void IntroState::Resume()
{
  Log::info("INTRO_STATE - Resuming state");
  Camera::GetInstance().SetPosition(0, 0);
}
