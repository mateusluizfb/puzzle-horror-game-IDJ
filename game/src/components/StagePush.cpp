#include "StagePush.h"
#include "Log.h"
#include "Game.h"
#include "TitleState.h"
#include "EndState.h"
#include "StageState.h"
#include "WaterTankPuzzleState.h"
#include "QuizState.h"
#include "Camera.h"
#include "InputManager.h"

StagePush::StagePush(GameObject& associated, const std::string& stageName)
  : Component(associated)
  , targetStage(stageName)
  , triggered(false)
{
  Log::debug("StagePush - Registered for object: " + associated.tag + 
             " -> target stage: " + targetStage);
}

void StagePush::Update(float /*dt*/) {
  triggered = false;
}

void StagePush::Render() {}

void StagePush::NotifyCollision(GameObject& other) {
  if (other.tag != "player" || triggered) return;

  InputManager& inputManager = InputManager::GetInstance();
  
  Log::info("StagePush - Transition to stage: " + targetStage);
  triggered = true;

  if (targetStage == "TitleState") {
    Game::GetInstance().Push(new TitleState());
    return;
  }

  if (targetStage == "EndState") {
    Game::GetInstance().Push(new EndState());
    return;
  }
  
  if (targetStage == "StageState") {
    Game::GetInstance().Push(new StageState());
    return;
  }

  if (targetStage == "WaterTankPuzzleState" && inputManager.KeyPress(E_KEY)) {
    Game::GetInstance().Push(new WaterTankPuzzleState());
    return;
  }

  if (targetStage == "Quiz" && inputManager.KeyPress(E_KEY)) {
    QuizState* quizState = new QuizState();
    quizState->SetIsOverlay();
    Game::GetInstance().Push(quizState);
    return;
  }

  Log::warning("StagePush - Unknown stage name: " + targetStage);
}
