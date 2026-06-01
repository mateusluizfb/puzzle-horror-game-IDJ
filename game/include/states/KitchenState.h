#ifndef KITCHENSTATE_H
#define KITCHENSTATE_H

#include "Music.h"
#include "CollisionSystem.h"
#include <memory>

class GameObject;

class KitchenState : public State
{
private:
  Music music;
  CollisionSystem collisionSystem;

public:
  KitchenState();
  ~KitchenState();

  void LoadAssets() override;
  void Start() override;
  void Pause() override;
  void Resume() override;
  void Update(float dt) override;
  void Render() override;
};

#endif
