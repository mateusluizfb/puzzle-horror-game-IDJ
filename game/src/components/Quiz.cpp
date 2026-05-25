#include "Game.h"
#include "GameData.h"
#include "Log.h"
#include "Quiz.h"
#include "CreateDialogueBox.h"
#include "InputManager.h"
#include "DialogueBox.h"

Quiz::Quiz(GameObject &associated, const std::vector<QuizData>& quizData)
  : Component(associated), quizData(quizData), currentQuestionIndex(0)
  {}

void Quiz::HandleQuizNotStarted() {
  currentQuestionIndex = 0;
  playerAnswers = {};
}

void Quiz::HandleQuizInProgress() {
  if (currentQuestionIndex >= static_cast<int>(quizData.size()))
  {
    Log::info("QUIZ - All questions answered, ending quiz");

    state = QuizState::Completed;
    return;
  }

  if (!GameData::dialogueActive) {
    Game &game = Game::GetInstance();
    State &currentState = game.GetCurrentState();

    Log::info("QUIZ - Starting quiz dialogue");

    QuizData currentQuestion = quizData[currentQuestionIndex];

    SDL_Color white = {255, 255, 255, 255};
    Rect box = Rect(
        0,
        Game::GetInstance().GetWindowHeight() / 1.5,
        Game::GetInstance().GetWindowWidth(),
        Game::GetInstance().GetWindowHeight() / 4);

    GameObject *go = CreateDialogueBox(
        box,
        "game/assets/font/neodgm.ttf", 24, white,
        currentQuestion.question,
        currentQuestion.options);

    currentState.AddObject(go);
    dialogueObject = currentState.GetObjectPtr(go);
    GameData::dialogueActive = true;
  }
}

void Quiz::HandleQuizCompleted() {
  if (!GameData::dialogueActive)
  {
    Game &game = Game::GetInstance();
    State &currentState = game.GetCurrentState();

    Log::info("QUIZ - Quiz completed dialogue finished, cleaning up");

    bool allCorrect = true;
    for (size_t i = 0; i < quizData.size(); i++)
    {
      if (i >= playerAnswers.size() || playerAnswers[i] != quizData[i].correctOptionIndex)
      {
        allCorrect = false;
        break;
      }
    }

    SDL_Color white = {255, 255, 255, 255};
    Rect box = Rect(
        0,
        Game::GetInstance().GetWindowHeight() / 1.5,
        Game::GetInstance().GetWindowWidth(),
        Game::GetInstance().GetWindowHeight() / 4);

    GameObject *go = CreateDialogueBox(
        box,
        "game/assets/font/neodgm.ttf", 24, white,
        allCorrect ? "Congratulations! You answered all questions correctly!" : "Quiz completed! Better luck next time.",
        {});

    currentState.AddObject(go);
    dialogueObject = currentState.GetObjectPtr(go);
    GameData::dialogueActive = true;

    state = QuizState::NotStarted;
    return;
  }
}

void Quiz::Update(float dt) {
  switch (state)
  {
    case QuizState::NotStarted:
      HandleQuizNotStarted();
      break;
    case QuizState::InProgress:
      HandleQuizInProgress();
      break;
    case QuizState::Completed:
      HandleQuizCompleted();
      break;
  }

  // Check if the dialogue box has finished and process the player's answer
  if (!dialogueObject.expired())
  {
    auto go = dialogueObject.lock();
    DialogueBox *db = go->GetComponent<DialogueBox>();

    if (db && db->IsFinished())
    {
      Log::info("STAGE_STATE - Dialogue finished");

      int selectedOption = db->GetSelectedOption();
      playerAnswers.push_back(selectedOption);
      Log::info("QUIZ - Player selected option " + std::to_string(selectedOption));

      currentQuestionIndex++;

      go->RequestDelete();
      GameData::dialogueActive = false;
      Log::info("QUIZ - Dialogue box deleted, moving to next question");
      return;
    }
  }
}

void Quiz::Render() {
}

void Quiz::NotifyCollision(GameObject &other) {
  if (other.tag != "player") return;

  InputManager &inputManager = InputManager::GetInstance();

  if (inputManager.KeyPress(E_KEY)) {
    Log::info("QUIZ - Player collided and pressed E, starting quiz");
    state = QuizState::InProgress;
  }
}
