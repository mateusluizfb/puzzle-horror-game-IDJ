#ifndef KEY_H
#define KEY_H

#include <string>
#include "Component.h"
#include "Text.h"

class Key : public Component {
public:
  enum class Type { Bedroom, LivingRoom };

  Key(GameObject& associated, Type type);
  void Update(float dt) override;
  void Render() override;
  void NotifyCollision(GameObject& other) override;

private:
  Type type;
  Text *keyText = nullptr;
  float timer = 0.0f;
  bool collected = false;
};

#endif
