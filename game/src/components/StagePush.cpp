#include "StagePush.h"
#include "Log.h"
#include "Game.h"
#include "Camera.h"
#include "InputManager.h"
#include "Text.h"

#include "TitleState.h"
#include "EndState.h"
#include "StageState.h"
#include "WarningState.h"
#include "WaterTankPuzzleState.h"
#include "LivingRoomCorridorState.h"
#include "LivingRoomState.h"
#include "KitchenCorridorState.h"
#include "KitchenState.h"
#include "QuizState.h"
#include "GameData.h"

StagePush::StagePush(GameObject& associated, const std::string& stageName)
  : Component(associated)
  , targetStage(stageName)
  , triggered(false)
{
  Log::debug("StagePush - Registered for object: " + associated.tag +
             " -> target stage: " + targetStage);

  GameObject* textGameObject = new GameObject();
  SDL_Color white = {255, 255, 255, 255};
  promptText = new Text(*textGameObject, "game/assets/font/neodgm.ttf", 32, Text::BLENDED, "Press E to open", white);
  promptText->Hide();
  textGameObject->box.x = (Game::GetInstance().GetWindowWidth() / 2) - 150;
  textGameObject->box.y = (Game::GetInstance().GetWindowHeight() - 100);
  associated.AddComponent(promptText);
}

void StagePush::Update(float /*dt*/) {
  if (isTouching) {
    promptText->Show();
  } else {
    promptText->Hide();
  }
  isTouching = false;
  triggered = false;
}

void StagePush::Render() {}

void StagePush::NotifyCollision(GameObject& other) {
  if (other.tag != "player" || triggered) return;

  if (targetStage != "TitleState" &&
      targetStage != "EndState"   &&
      targetStage != "StageState" &&
      targetStage != "MazeState") {
    isTouching = true;
  }

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

  if (targetStage == "WarningState" && inputManager.KeyPress(E_KEY) && GameData::hasLivingRoomKey)
  {
    Game::GetInstance().Push(new WarningState());
	  return;
  }

  if (targetStage == "MazeState") {
    Game::GetInstance().Push(new MazeState());
	return;
  }

  if (targetStage == "Pop" && inputManager.KeyPress(E_KEY))
  {
    Log::info("STAGE_PUSH - Requesting current state pop");
    Game::GetInstance().GetCurrentState().RequestPop();
    return;
  }

  if (targetStage == "LivingRoomCorridorState" && inputManager.KeyPress(E_KEY) && GameData::hasBedroomKey)
  {
    Game::GetInstance().Push(new LivingRoomCorridorState());
    return;
  }

  if (targetStage == "LivingRoomState" && inputManager.KeyPress(E_KEY))
  {
    Game::GetInstance().Push(new LivingRoomState());
    return;
  }

  if (targetStage == "KitchenCorridorState" && inputManager.KeyPress(E_KEY))
  {
    Game::GetInstance().Push(new KitchenCorridorState());
    return;
  }

  if (targetStage == "KitchenState" && inputManager.KeyPress(E_KEY))
  {
    Game::GetInstance().Push(new KitchenState());
    return;
  }

  if (targetStage == "WaterTankPuzzleState" && inputManager.KeyPress(E_KEY)) {
    Game::GetInstance().Push(new WaterTankPuzzleState());
    return;
  }

  if (targetStage == "QuizState" && inputManager.KeyPress(E_KEY)) {
    QuizState* quizState = new QuizState();
    quizState->SetIsOverlay();
    Game::GetInstance().Push(quizState);
    return;
  }

  Log::warning("StagePush - Unknown stage name: " + targetStage);
}
