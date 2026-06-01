#ifndef QUIZ_H
#define QUIZ_H

#include <string>
#include <vector>
#include "Component.h"

struct QuizData {
  std::string question;
  std::vector<std::string> options;
  int correctOptionIndex;
};

enum class QuizProgressState
{
  NotStarted,
  InProgress,
  Completed,
  Finished
};

class Quiz : public Component
{
public:
  Quiz(GameObject &associated, const std::vector<QuizData>& quizData);

  void Update(float dt) override;
  void Render() override;

  void StartQuiz();
  void SubmitAnswer(int selectedOption);
  void Finish();

  QuizProgressState GetState() const;
  const QuizData& GetCurrentQuestion() const;
  bool IsAllCorrect() const;

private:
  std::vector<QuizData> quizData;
  std::vector<int> playerAnswers;
  int currentQuestionIndex;
  QuizProgressState state = QuizProgressState::NotStarted;

  void HandleQuizNotStarted();
};

#endif
