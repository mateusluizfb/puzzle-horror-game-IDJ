#ifndef LIVINGROOMSTATE_H
#define LIVINGROOMSTATE_H

#include "Music.h"
#include "CollisionSystem.h"
#include <memory>

class GameObject;

class LivingRoomState : public State
{
private:
  Music music;
  CollisionSystem collisionSystem;

public:
  LivingRoomState();
  ~LivingRoomState();

  void LoadAssets() override;
  void Start() override;
  void Pause() override;
  void Resume() override;
  void Update(float dt) override;
  void Render() override;
};

#endif
