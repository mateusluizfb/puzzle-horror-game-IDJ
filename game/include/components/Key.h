#ifndef KEY_H
#define KEY_H

#include <string>
#include "Component.h"
#include "Text.h"

class Key : public Component {
private:
  Text *keyText = nullptr;
  float timer = 0.0f;
  bool collected = false;

public:
  Key(GameObject& associated);
  void Update(float dt) override;
  void Render() override;
  void NotifyCollision(GameObject& other) override;
};

#endif