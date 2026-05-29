#ifndef WATER_PIPE_H
#define WATER_PIPE_H

#include <string>
#include "Component.h"
#include "Vec2.h"

class WaterPipe : public Component
{
private:
  enum class State
  {
    LV0,
    LV1,
    LV2,
    LV3,
    LOOPING
  };

  int level = 0;

public:
  WaterPipe(GameObject &associated);

  void Update(float dt) override;
  void Render() override;

  void LevelUp();
  void LevelDown();
  int GetLevel();
};

#endif
