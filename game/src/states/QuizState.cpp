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
      "What is the capital of France?",
      {"Paris", "London", "Berlin"},
      0
    },
    {
      "What is 2 + 2?",
      {"3", "4", "5"},
      1
    },
    {
      "Which planet is known as the Red Planet?",
      {"Earth", "Mars", "Jupiter"},
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
      this->RequestPop();
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
