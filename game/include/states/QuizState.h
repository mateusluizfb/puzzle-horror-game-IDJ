#ifndef QUIZSTATE_H
#define QUIZSTATE_H

#include "Music.h"
#include "State.h"
#include "Quiz.h"

class QuizState : public State
{
private:
  Music music;
public:
  QuizState();
  ~QuizState();

  void LoadAssets() override;
  void Update(float dt) override;
  void Render() override;
  void Start() override;
  void Pause() override;
  void Resume() override;
};

#endif
