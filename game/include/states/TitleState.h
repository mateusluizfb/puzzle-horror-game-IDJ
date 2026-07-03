#ifndef TITLESTATE_H
#define TITLESTATE_H

#include "Music.h"
#include "State.h"
#include <memory>

class TitleState : public State {
private:
  Music music;
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
