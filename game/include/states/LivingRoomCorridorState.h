#ifndef LIVINGROOMCORRIDORSTATE_H
#define LIVINGROOMCORRIDORSTATE_H

#include "Music.h"
#include "CollisionSystem.h"
#include <memory>

class GameObject;

class LivingRoomCorridorState : public State
{
private:
  Music music;
  CollisionSystem collisionSystem;

public:
  LivingRoomCorridorState();
  ~LivingRoomCorridorState();

  void LoadAssets() override;
  void Start() override;
  void Pause() override;
  void Resume() override;
  void Update(float dt) override;
  void Render() override;
};

#endif
