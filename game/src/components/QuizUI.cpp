#include "Game.h"
#include "GameData.h"
#include "Log.h"
#include "QuizUI.h"
#include "Quiz.h"
#include "CreateDialogueBox.h"
#include "DialogueBox.h"

QuizUI::QuizUI(GameObject &associated)
  : Component(associated), resultShown(false), introShown(false)
  {}

void QuizUI::ShowIntro(Quiz* quiz) {
  Game &game = Game::GetInstance();
  State &currentState = game.GetCurrentState();

  Log::info("QUIZ_UI - Showing intro dialogue");

  SDL_Color white = {255, 255, 255, 255};
  Rect box = Rect(
      0,
      Game::GetInstance().GetWindowHeight() / 1.5,
      Game::GetInstance().GetWindowWidth(),
      Game::GetInstance().GetWindowHeight() / 4);

  GameObject *go = CreateDialogueBox(
      box,
      "game/assets/font/neodgm.ttf", 24, white,
      "Você quer água é?? Hihihih, vamos ver se você prestou atenção... hihihi...",
      {},
      DialogueBox::PortraitMode::RAT_QUIZ);

  currentState.AddObject(go);
  dialogueObject = currentState.GetObjectPtr(go);
  GameData::dialogueActive = true;
  introShown = true;
}

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
      question.options,
      DialogueBox::PortraitMode::RAT_QUIZ);

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
      quiz->IsAllCorrect() ? "Ora ora, então você é o bichão mesmo ein..." : "Infelizmente você vai .&@#$%@????++___As!@#%%!@#!!@#¨:!!!@@##:):)sedesedesedesedesedesedesedesede_sede",
      {},
      DialogueBox::PortraitMode::RAT_QUIZ);

  currentState.AddObject(go);
  dialogueObject = currentState.GetObjectPtr(go);
  GameData::dialogueActive = true;
  resultShown = true;
}

void QuizUI::Update(float dt) {
  Quiz* quiz = associated.GetComponent<Quiz>();
  if (!quiz) return;

  if (!GameData::dialogueActive && dialogueObject.expired()) {
    QuizProgressState state = quiz->GetState();

    Log::debug("QUIZ_UI - Quiz state: " + std::to_string(static_cast<int>(state)));

    if (state == QuizProgressState::Intro && !introShown) {
      ShowIntro(quiz);
      return;
    }

    if (state == QuizProgressState::InProgress) {
      ShowQuestion(quiz);
      return;
    }

    if (state == QuizProgressState::Completed && !resultShown) {
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
        quiz->Finish();
        resultShown = false;
      } else if (introShown) {
        Log::info("QUIZ_UI - Intro dialogue finished, starting quiz");
        quiz->StartQuiz();
        introShown = false;
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
