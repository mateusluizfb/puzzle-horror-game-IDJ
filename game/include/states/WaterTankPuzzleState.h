
#ifndef WATER_TANK_PUZZLE_STATE
#define WATER_TANK_PUZZLE_STATE

#include "State.h"
#include "Music.h"

class WaterTankPuzzleState : public State
{
public:
  WaterTankPuzzleState();
  ~WaterTankPuzzleState();
  void LoadAssets() override;
  void Update(float dt) override;
  void Render() override;
  void Start() override;
  void Pause() override;
  void Resume() override;

private:
  Music backgroundMusic;
};

#endif
