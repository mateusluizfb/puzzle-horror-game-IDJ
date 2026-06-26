#include "Game.h"
#include "QuizState.h"
#include "TitleState.h"
#include "GameData.h"
#include "SpriteRenderer.h"
#include "Log.h"
#include "InputManager.h"
#include "Camera.h"
#include "TileMap.h"
#include "TileSet.h"
#include "TileObjects.h"
#include "Quiz.h"
#include "QuizUI.h"
#include "EndState.h"

QuizState::QuizState() : State()
{
  Log::info("PIPE_PUZZLE_STATE - Initializing QuizState");
}

QuizState::~QuizState()
{
  Log::info("PIPE_PUZZLE_STATE - Destroying QuizState");

  objectArray.clear();
}

void QuizState::LoadAssets()
{
  Log::info("PIPE_PUZZLE_STATE - Loading assets for QuizState");

  std::vector<QuizData> quizData = {
    {
      "Esse santo foi versado em várias áreas da língua e o primeiro a transcrever o Tupi, ajudou a fundar São Paulo e Rio de Janeiro.",
      {"José de Anchieta", "São Francisco", "São José"},
      0
    },
    {
      "Esse missionário foi morto por aqueles a quem ensinava a piedade e a compaixão cristã.",
      {"Judas", "William Carey", "Juan del Castillo"},
      2
    },
    {
      "Este santo brasileiro, cercado de rumores sobrenaturais, ficou conhecido por suas pílulas milagrosas.",
      {"Frei Gilson", "Frei Galvão", "Frei Damião"},
      1
    }
  };

  GameObject *quizObject = new GameObject();
  QuizUI *quizUI = new QuizUI(*quizObject);
  Quiz *quiz = new Quiz(*quizObject, quizData);
  quizObject->AddComponent(quiz);
  quizObject->AddComponent(quizUI);
  this->AddObject(quizObject);
  quiz->StartQuiz();
}

void QuizState::Update(float dt)
{
  InputManager& inputManager = InputManager::GetInstance();

  std::shared_ptr<GameObject> quizObject = GetObjectByTag("quiz").lock();
  if (quizObject) {
    Quiz* quiz = quizObject->GetComponent<Quiz>();
    if (quiz && quiz->GetState() == QuizProgressState::Finished) {
      Log::info("QUIZ_STATE - Quiz finished, returning to TitleState");

      if (quiz && quiz->IsAllCorrect())
      {
        GameData::playerVictory = true;
        Game::GetInstance().Push(new EndState());
      } else {
        GameData::playerVictory = false;
        Game::GetInstance().Push(new EndState());
      }
    }
  }

  if (inputManager.KeyPress(ESCAPE_KEY))
  {
    this->RequestPop();
  }

  UpdateArray(dt);
  UpdateDead();
}

void QuizState::Render()
{
  RenderArray();
}

void QuizState::Start() {
  StartArray();
}
void QuizState::Pause() {}
void QuizState::Resume() {}
