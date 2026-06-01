#ifndef DEBUGSTATE_H
#define DEBUGSTATE_H

#include "State.h"

class DebugState : public State
{
public:
  Vec2 cameraPosition = Vec2(0, 0);

  DebugState();
  ~DebugState();

  void LoadAssets() override;
  void Update(float dt) override;
  void Render() override;
  void Start() override;
  void Pause() override;
  void Resume() override;
};

#endif
