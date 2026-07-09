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
#include "KitchenState.h"
#include "QuizState.h"
#include "GameData.h"
#include "GlobalSounds.h"

StagePush::StagePush(GameObject& associated, const std::string& stageName)
  : Component(associated)
  , targetStage(stageName)
  , triggered(false)
{
  Log::debug("StagePush - Registered for object: " + associated.tag +
             " -> target stage: " + targetStage);

  SDL_Color white = {255, 255, 255, 255};

  GameObject* textGameObject = new GameObject();
  promptText = new Text(*textGameObject, "game/assets/font/neodgm.ttf", 32, Text::BLENDED, "Aperte E para abrir", white);
  promptText->Hide();
  textGameObject->box.x = (Game::GetInstance().GetWindowWidth() / 2) - 150;
  textGameObject->box.y = (Game::GetInstance().GetWindowHeight() - 100);
  associated.AddComponent(promptText);

  GameObject* lockedTextGameObject = new GameObject();
  lockedPromptText = new Text(*lockedTextGameObject, "game/assets/font/neodgm.ttf", 32, Text::BLENDED, "Porta trancada", white);
  lockedPromptText->Hide();
  lockedTextGameObject->box.x = (Game::GetInstance().GetWindowWidth() / 2) - 150;
  lockedTextGameObject->box.y = (Game::GetInstance().GetWindowHeight() - 100);
  associated.AddComponent(lockedPromptText);

}

void StagePush::Update(float /*dt*/) {
  bool locked = (targetStage == "Locked") ||
                (targetStage == "WarningState" && !GameData::hasLivingRoomKey) ||
                (targetStage == "LivingRoomCorridorState" && !GameData::hasBedroomKey);

	// O SISTEMA DE TRAVA COM PRIORIDADE (PORTA = NIVEL 3)
    int myPriority = 3;
	bool canTakeFocus = (GameData::activeInteraction == nullptr ||
                         GameData::activeInteraction == this ||
                         myPriority > GameData::interactionPriority);

  if (isTouching && canTakeFocus) {
	  GameData::activeInteraction = this;
	  GameData::interactionPriority = myPriority;
	  if (locked) {
		  lockedPromptText->Show();
		  promptText->Hide();
	  } else {
		  promptText->Show();
		  lockedPromptText->Hide();
	  }
  } else {
        if (GameData::activeInteraction == this) {
            GameData::activeInteraction = nullptr;
            GameData::interactionPriority = 0;
        }
		promptText->Hide();
		lockedPromptText->Hide();
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

  if (GameData::activeInteraction == this) {
            GameData::activeInteraction = nullptr;
            GameData::interactionPriority = 0;
        }

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
    GlobalSounds::Door().Play(0);
    Game::GetInstance().Push(new WarningState());
	  return;
  }

  if (targetStage == "MazeState") {
    Game::GetInstance().Push(new MazeState());
	return;
  }

  if (targetStage == "Pop" && inputManager.KeyPress(E_KEY))
  {
    GlobalSounds::Door().Play(0);
    Log::info("STAGE_PUSH - Requesting current state pop");
    Game::GetInstance().GetCurrentState().RequestPop();
    return;
  }

  if (targetStage == "LivingRoomCorridorState" && inputManager.KeyPress(E_KEY) && GameData::hasBedroomKey)
  {
    GlobalSounds::Door().Play(0);
    Game::GetInstance().Push(new LivingRoomCorridorState());
    return;
  }

  if (targetStage == "LivingRoomState" && inputManager.KeyPress(E_KEY))
  {
    GlobalSounds::Door().Play(0);
    Game::GetInstance().Push(new LivingRoomState());
    return;
  }

  if (targetStage == "KitchenState" && inputManager.KeyPress(E_KEY))
  {
    GlobalSounds::Door().Play(0);
    Game::GetInstance().Push(new KitchenState());
    return;
  }

  if (targetStage == "WaterTankPuzzleState" && inputManager.KeyPress(E_KEY)) {
    GlobalSounds::Door().Play(0);
    Game::GetInstance().Push(new WaterTankPuzzleState());
    return;
  }

  if (targetStage == "QuizState" && inputManager.KeyPress(E_KEY)) {
    GlobalSounds::Door().Play(0);
    QuizState* quizState = new QuizState();
    quizState->SetIsOverlay();
    Game::GetInstance().Push(quizState);
    return;
  }

  Log::warning("StagePush - Unknown stage name: " + targetStage);
}
