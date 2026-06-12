#include "Camera.h"
#include "InputManager.h"
#include "Game.h"
#include "Log.h"

Camera& Camera::GetInstance() {
  static Camera instance; 
  return instance;
}

void Camera::Follow(GameObject* newFocus) {
  focus = newFocus;
}

void Camera::Unfollow() {
  focus = nullptr;
}

void Camera::SetPosition(float x, float y) {
  pos.x = x;
  pos.y = y;
}

void Camera::SetSpeed(float x, float y) {
  speed.x = x;
  speed.y = y;
}

void Camera::SetSpeedMultiplier(float speedMultiplier) {
  this->speedMultiplier = speedMultiplier;
}

void Camera::ResetSpeedMultiplier() {
  speedMultiplier = 1.0f;
}

float Camera::GetPositionX() {
  return pos.x * speedMultiplier;
}

float Camera::GetPositionY()
{
  return pos.y * speedMultiplier;
}

Vec2 Camera::GetCenterPosition()
{
  if (focus == nullptr) {
    return Vec2(
      pos.x + (Game::GetInstance().GetWindowWidth() / 2),
      pos.y + (Game::GetInstance().GetWindowHeight() / 2)
    );
  }

  return focus->box.GetCenter();
}

void Camera::Update(float dt) {
  if (focus != nullptr) {
    Vec2 focusCenter = focus->box.GetCenter();
    
    pos.x = focusCenter.x - (Game::GetInstance().GetWindowWidth() / 2);
    pos.y = focusCenter.y - (Game::GetInstance().GetWindowHeight() / 2);
    
    return;
  }
}
