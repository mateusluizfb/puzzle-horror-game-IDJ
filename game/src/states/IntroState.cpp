#include "IntroState.h"

IntroState::IntroState() : State(), transitioned(false), currentIntroFrame(0), introFinished(false), bgGameObject(nullptr)
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
  Log::debug("INTRO_STATE - Starting black background game object");
  GameObject *blackBgGameObject = new GameObject();
  blackBgGameObject->AddComponent(new SpriteRenderer(*blackBgGameObject, "game/assets/img/black_solid_color.png"));
  SpriteRenderer *blackBgSprite = blackBgGameObject->GetComponent<SpriteRenderer>();
  blackBgSprite->SetCameraFollower(true);
  blackBgSprite->SetPosition(0, 0);
  this->AddObject(blackBgGameObject);
  Log::debug("INTRO_STATE - Black background game object loaded");

  Log::debug("INTRO_STATE - Starting background game object");
  bgGameObject = new GameObject();
  bgGameObject->AddComponent(new SpriteRenderer(*bgGameObject, "game/assets/img/intro/intro_0.png"));
  SpriteRenderer *bgSprite = bgGameObject->GetComponent<SpriteRenderer>();
  bgSprite->SetCameraFollower(true);
  bgSprite->SetScale(0.50f, 0.50f);
  bgSprite->SetPosition(0, 75);
  this->AddObject(bgGameObject);
  Log::debug("INTRO_STATE - Background game object loaded");
}

void IntroState::SpawnDialogue()
{
  Log::debug("INTRO_STATE - Starting dialogue box game object");
  Rect box = Rect(
      0,
      Game::GetInstance().GetWindowHeight() / 1.5,
      Game::GetInstance().GetWindowWidth(),
      Game::GetInstance().GetWindowHeight() / 4);
  SDL_Color white = {255, 255, 255, 255};
  GameObject *dialogueGameObject = CreateDialogueBox(box, "game/assets/font/neodgm.ttf", 32, white, "Nossa que sede, vou na cozinha... Mas que sensação estranha... o que é isso?", {}, DialogueBox::PortraitMode::STILL);
  this->AddObject(dialogueGameObject);
  Log::debug("INTRO_STATE - Dialogue box game object loaded");
}

void IntroState::FinishIntro()
{
  introFinished = true;
  SpriteRenderer *bgSprite = bgGameObject->GetComponent<SpriteRenderer>();
  bgSprite->Open("game/assets/img/black_solid_color.png");
  bgSprite->SetScale(1.0f, 1.0f);
  SpawnDialogue();
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

  if (!introFinished)
  {
    if (inputManager.KeyPress(SPACE_KEY))
    {
      Log::info("INTRO_STATE - Space pressed, skipping intro cutscene");
      FinishIntro();
    }
    else
    {
      introTimer.Update(dt);
      while (introTimer.Get() >= 1.0f)
      {
        introTimer.Restart();
        ++currentIntroFrame;
        if (currentIntroFrame < 5)
        {
          SpriteRenderer *bgSprite = bgGameObject->GetComponent<SpriteRenderer>();
          bgSprite->Open("game/assets/img/intro/intro_" + std::to_string(currentIntroFrame) + ".png");
        }
        else
        {
          FinishIntro();
        }
      }
    }
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
