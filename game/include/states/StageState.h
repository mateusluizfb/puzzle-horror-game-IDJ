#ifndef STAGESTATE_H
#define STAGESTATE_H

#include "Music.h"
#include "CollisionSystem.h"

class StageState : public State
{
private:
  Music music;
  CollisionSystem collisionSystem;

public:
  StageState();
  ~StageState();

  void LoadAssets() override;
  void Start() override;
  void Pause() override;
  void Resume() override;
  void Update(float dt) override;
  void Render() override;
};

#endif
