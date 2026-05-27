#include "Game.h"
#include "GameData.h"
#include "Log.h"
#include "QuizUI.h"
#include "Quiz.h"
#include "CreateDialogueBox.h"
#include "DialogueBox.h"

QuizUI::QuizUI(GameObject &associated)
  : Component(associated), resultShown(false)
  {}

void QuizUI::ShowQuestion(Quiz* quiz) {
  Game &game = Game::GetInstance();
  State &currentState = game.GetCurrentState();

  Log::info("QUIZ_UI - Showing question dialogue");

  const QuizData& question = quiz->GetCurrentQuestion();

  SDL_Color white = {255, 255, 255, 255};
  Rect box = Rect(
      0,
      Game::GetInstance().GetWindowHeight() / 1.5,
      Game::GetInstance().GetWindowWidth(),
      Game::GetInstance().GetWindowHeight() / 4);

  GameObject *go = CreateDialogueBox(
      box,
      "game/assets/font/neodgm.ttf", 24, white,
      question.question,
      question.options);

  currentState.AddObject(go);
  dialogueObject = currentState.GetObjectPtr(go);
  GameData::dialogueActive = true;
}

void QuizUI::ShowResult(Quiz* quiz) {
  Game &game = Game::GetInstance();
  State &currentState = game.GetCurrentState();

  Log::info("QUIZ_UI - Showing result dialogue");

  SDL_Color white = {255, 255, 255, 255};
  Rect box = Rect(
      0,
      Game::GetInstance().GetWindowHeight() / 1.5,
      Game::GetInstance().GetWindowWidth(),
      Game::GetInstance().GetWindowHeight() / 4);

  GameObject *go = CreateDialogueBox(
      box,
      "game/assets/font/neodgm.ttf", 24, white,
      quiz->IsAllCorrect() ? "Congratulations! You answered all questions correctly!" : "Quiz completed! Better luck next time.",
      {});

  currentState.AddObject(go);
  dialogueObject = currentState.GetObjectPtr(go);
  GameData::dialogueActive = true;
  resultShown = true;
}

void QuizUI::Update(float dt) {
  Quiz* quiz = associated.GetComponent<Quiz>();
  if (!quiz) return;

  if (!GameData::dialogueActive && dialogueObject.expired()) {
    QuizState state = quiz->GetState();

    if (state == QuizState::InProgress) {
      ShowQuestion(quiz);
      return;
    }

    if (state == QuizState::Completed && !resultShown) {
      ShowResult(quiz);
      return;
    }
  }

  if (!dialogueObject.expired()) {
    auto go = dialogueObject.lock();
    DialogueBox *db = go->GetComponent<DialogueBox>();

    if (db && db->IsFinished()) {
      int selectedOption = db->GetSelectedOption();

      if (resultShown) {
        Log::info("QUIZ_UI - Result dialogue finished");
        quiz->Reset();
        resultShown = false;
      } else {
        Log::info("QUIZ_UI - Question dialogue finished, submitting answer");
        quiz->SubmitAnswer(selectedOption);
      }

      go->RequestDelete();
      GameData::dialogueActive = false;
    }
  }
}

void QuizUI::Render() {
}
