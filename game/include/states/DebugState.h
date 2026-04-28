#ifndef DEBUGSTATE_H
#define DEBUGSTATE_H

#include "Music.h"
#include "CollisionSystem.h"

class DebugState : public State
{
private:
  Music music;
  CollisionSystem collisionSystem;

public:
  DebugState();
  ~DebugState();

  void LoadAssets() override;
  void Start() override;
  void Pause() override;
  void Resume() override;
  void Update(float dt) override;
  void Render() override;
};

#endif
