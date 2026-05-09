#include "Log.h"
#include "Character.h"
#include "PlayerController.h"
#include "InputManager.h"
#include "Vec2.h"

PlayerController::PlayerController(GameObject& gameObject)
  : Component(gameObject) {}

void PlayerController::Start() {
  // TODO
}

void PlayerController::Update(float dt) {
  InputManager &inputManager = InputManager::GetInstance();
  Character* character = associated.GetComponent<Character>();

  if (inputManager.IsKeyDown(LEFT_ARROW_KEY)) {
    character->Issue(Character::Command(CommandType::MOVE, -1, 0));  
  }

  if (inputManager.IsKeyDown(RIGHT_ARROW_KEY)) {
    character->Issue(Character::Command(CommandType::MOVE, 1, 0));
  }

  if (inputManager.IsKeyDown(UP_ARROW_KEY)) {
    character->Issue(Character::Command(CommandType::MOVE, 0, -1));
  }

  if (inputManager.IsKeyDown(DOWN_ARROW_KEY)) {
    character->Issue(Character::Command(CommandType::MOVE, 0, 1));
  }

  if (inputManager.MousePress(LEFT_MOUSE_BUTTON))
  {
    character->Issue(Character::Command(CommandType::SHOOT, inputManager.GetMouseXWorld(), inputManager.GetMouseYWorld()));
  }
}

void PlayerController::Render() {
}
