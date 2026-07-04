#ifndef TITLESTATE_H
#define TITLESTATE_H

#include "Music.h"
#include "Sound.h"
#include "State.h"
#include "Timer.h"
#include <memory>

class TitleState : public State {
private:
  Music music;
  Sound cupBreakSound;
  Timer blinkTimer;
  std::weak_ptr<GameObject> promptText;
  bool transitioning;
  bool textVisible;
  float blinkAccumulator;
public:
    TitleState();
    ~TitleState();

    void LoadAssets() override;
    void Update(float dt) override;
    void Render() override;
    void Start() override;
    void Pause() override;
    void Resume() override;
};

#endif
