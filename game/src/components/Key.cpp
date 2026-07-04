#include "Key.h"
#include "Log.h"
#include "GameObject.h"
#include "Game.h"
#include "TileObject.h"
#include "GameData.h"

Key::Key(GameObject& associated, Key::Type type)
  : Component(associated) {
  this->type = type;
  associated.tag = "key";

  GameObject *textGameObject = new GameObject();
  SDL_Color white = {255, 255, 255, 255};
  keyText = new Text(*textGameObject, "game/assets/font/neodgm.ttf", 32,
                     Text::BLENDED, "Pegou a chave", white);
  keyText->Hide();
  textGameObject->box.x = (Game::GetInstance().GetWindowWidth() / 2) - 150;
  textGameObject->box.y = (Game::GetInstance().GetWindowHeight() - 100);
  associated.AddComponent(keyText);
}

void Key::Update(float dt) {
  if (collected) {
    timer += dt;
    if (timer >= 3.0f) {
      keyText->Hide();
      associated.RequestDelete();
    }
  }
}

void Key::Render() {}

void Key::NotifyCollision(GameObject& other) {
  if (collected) return;
  
  if (other.tag == "player" && this->type == Type::Bedroom) {
    collected = true;
    GameData::hasBedroomKey = true;
    keyText->Show();
    TileObject *tile = associated.GetComponent<TileObject>();
    if (tile) tile->Hide();
    Log::info("KEY - Player collected the bedroom key");
  }

  if (other.tag == "player" && this->type == Type::LivingRoom) {
    collected = true;
    GameData::hasLivingRoomKey = true;
    keyText->Show();
    TileObject *tile = associated.GetComponent<TileObject>();
    if (tile) tile->Hide();
    Log::info("KEY - Player collected the living room key");
  }
}
