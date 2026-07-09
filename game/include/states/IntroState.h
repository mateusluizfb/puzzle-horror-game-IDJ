#ifndef INTROSTATE_H
#define INTROSTATE_H

#include "State.h"
#include "Log.h"
#include "Game.h"
#include "GameObject.h"
#include "InputManager.h"
#include "Timer.h"
#include "Camera.h"
#include "BedroomState.h"
#include "CreateDialogueBox.h"
#include "DialogueBox.h"
#include "SpriteRenderer.h"
#include "Resources.h"

class IntroState : public State {
private:
  Music music;
  bool transitioned;
  int currentIntroFrame;
  Timer introTimer;
  bool introFinished;
  GameObject* bgGameObject;

  void SpawnDialogue();
  void FinishIntro();

public:
  IntroState();
  ~IntroState();

  void LoadAssets() override;
  void Start() override;
  void Pause() override;
  void Resume() override;
  void Update(float dt) override;
  void Render() override;
};

#endif
