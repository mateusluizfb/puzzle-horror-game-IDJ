#ifndef QUIZ_UI_H
#define QUIZ_UI_H

#include "Component.h"
#include <memory>

class GameObject;
class Quiz;

class QuizUI : public Component
{
public:
  QuizUI(GameObject &associated);

  void Update(float dt) override;
  void Render() override;

private:
  std::weak_ptr<GameObject> dialogueObject;
  bool resultShown;

  void ShowQuestion(Quiz* quiz);
  void ShowResult(Quiz* quiz);
};

#endif
