#ifndef QUIZ_H
#define QUIZ_H

#include <string>
#include <vector>
#include "Component.h"
#include "Vec2.h"

struct QuizData {
  std::string question;
  std::vector<std::string> options;
  int correctOptionIndex;
};

class Quiz : public Component
{
public:
  enum class QuizState
  {
    NotStarted,
    InProgress,
    Completed
  };

  Quiz(GameObject &associated, const std::vector<QuizData>& quizData);

  void Update(float dt) override;
  void Render() override;
  void NotifyCollision(GameObject &other) override;

private:
  std::vector<QuizData> quizData;
  std::vector<int> playerAnswers;
  int currentQuestionIndex;
  std::weak_ptr<GameObject> dialogueObject;
  QuizState state = QuizState::NotStarted;

  void HandleQuizNotStarted();
  void HandleQuizInProgress();
  void HandleQuizCompleted();
};

#endif
