#ifndef BEDROOMSTATE_H
#define BEDROOMSTATE_H

#include "Music.h"
#include "CollisionSystem.h"
#include <memory>

class GameObject;

class BedroomState : public State
{
private:
  Music music;
  CollisionSystem collisionSystem;

public:
  BedroomState();
  ~BedroomState();

  void LoadAssets() override;
  void Start() override;
  void Pause() override;
  void Resume() override;
  void Update(float dt) override;
  void Render() override;
};

#endif
