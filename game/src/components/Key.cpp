#include "Key.h"
#include "Log.h"
#include "GameObject.h"
#include "Game.h"
#include "TileObject.h"
#include "GameData.h"

Key::Key(GameObject& associated)
  : Component(associated) {
  associated.tag = "key";

  GameObject *textGameObject = new GameObject();
  SDL_Color white = {255, 255, 255, 255};
  keyText = new Text(*textGameObject, "game/assets/font/neodgm.ttf", 32,
                     Text::BLENDED, "You got the key", white);
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
  
  if (other.tag == "player") {
    collected = true;
    GameData::hasLivingRoomKey = true;
    keyText->Show();
    TileObject *tile = associated.GetComponent<TileObject>();
    if (tile) tile->Hide();
    Log::info("KEY - Player collected the living room key");
  }
}
