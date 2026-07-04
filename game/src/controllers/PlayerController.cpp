#include "Log.h"
#include "Character.h"
#include "PlayerController.h"
#include "InputManager.h"
#include "Vec2.h"
#include "GameData.h"

PlayerController::PlayerController(GameObject& gameObject)
  : Component(gameObject) {}

void PlayerController::Start() {
}

void PlayerController::Update(float dt) {
  InputManager &inputManager = InputManager::GetInstance();
  Character* character = associated.GetComponent<Character>();
  
  if (GameData::dialogueActive) {
    return;
  }
  
  if (inputManager.IsKeyDown(SDLK_a) || inputManager.IsKeyDown(LEFT_ARROW_KEY)) {
    character->Issue(Character::Command(CommandType::MOVE, -1, 0));
  }

  if (inputManager.IsKeyDown(SDLK_d) || inputManager.IsKeyDown(RIGHT_ARROW_KEY)) {
    character->Issue(Character::Command(CommandType::MOVE, 1, 0));
  }

  if (inputManager.IsKeyDown(SDLK_w) || inputManager.IsKeyDown(UP_ARROW_KEY)) {
    character->Issue(Character::Command(CommandType::MOVE, 0, -1));
  }

  if (inputManager.IsKeyDown(SDLK_s) || inputManager.IsKeyDown(DOWN_ARROW_KEY)) {
    character->Issue(Character::Command(CommandType::MOVE, 0, 1));
  }

  Log::debug("Mouse: " + std::to_string(inputManager.GetMouseX()) + "x" + std::to_string(inputManager.GetMouseY()));
}

void PlayerController::Render() {
}
