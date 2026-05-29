#include "Game.h"
#include "Log.h"
#include "Quiz.h"
#include "InputManager.h"
#include "GameData.h"

Quiz::Quiz(GameObject &associated, const std::vector<QuizData>& quizData)
  : Component(associated), quizData(quizData), currentQuestionIndex(0)
  {
    associated.tag = "quiz";
  }

void Quiz::HandleQuizNotStarted() {
  currentQuestionIndex = 0;
  playerAnswers = {};
}

void Quiz::StartQuiz() {
  Log::info("QUIZ - Starting quiz");
  currentQuestionIndex = 0;
  playerAnswers.clear();
  state = QuizProgressState::InProgress;
  GameData::dialogueActive = false;
}

void Quiz::SubmitAnswer(int selectedOption) {
  playerAnswers.push_back(selectedOption);
  Log::info("QUIZ - Player answered option " + std::to_string(selectedOption));

  currentQuestionIndex++;

  if (currentQuestionIndex >= static_cast<int>(quizData.size())) {
    Log::info("QUIZ - All questions answered, ending quiz");
    state = QuizProgressState::Completed;
  }
}

QuizProgressState Quiz::GetState() const {
  return state;
}

const QuizData& Quiz::GetCurrentQuestion() const {
  return quizData[currentQuestionIndex];
}

bool Quiz::IsAllCorrect() const {
  for (size_t i = 0; i < quizData.size(); i++) {
    if (i >= playerAnswers.size() || playerAnswers[i] != quizData[i].correctOptionIndex) {
      return false;
    }
  }
  return true;
}

void Quiz::Update(float dt) {
  if (state == QuizProgressState::NotStarted) {
    HandleQuizNotStarted();
  }
}

void Quiz::Finish() {
  Log::info("QUIZ - Resetting quiz");
  state = QuizProgressState::Finished;
}

void Quiz::Render() {
}
