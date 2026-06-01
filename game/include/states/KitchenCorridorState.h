#ifndef KITCHENCORRIDORSTATE_H
#define KITCHENCORRIDORSTATE_H

#include "Music.h"
#include "CollisionSystem.h"
#include <memory>

class GameObject;

class KitchenCorridorState : public State
{
private:
  Music music;
  CollisionSystem collisionSystem;

public:
  KitchenCorridorState();
  ~KitchenCorridorState();

  void LoadAssets() override;
  void Start() override;
  void Pause() override;
  void Resume() override;
  void Update(float dt) override;
  void Render() override;
};

#endif
