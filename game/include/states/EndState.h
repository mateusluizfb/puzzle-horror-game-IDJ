
#ifndef END_STATE
#define END_STATE

#include <memory>
#include "State.h"
#include "Music.h"
#include "Sound.h"
#include "Timer.h"

class GameObject;

class EndState : public State {
  public:
    EndState();
    ~EndState();
    void LoadAssets() override;
    void Update(float dt) override;
    void Render() override;
    void Start() override;
    void Pause() override;
    void Resume() override;

  private:
    Sound backgroundMusic;
    bool playMusic = false;

    Timer jumpscareTimer;
    bool jumpscareActive = false;
    Sound scream;
    std::weak_ptr<GameObject> exorcistGameObject;
    std::weak_ptr<GameObject> gameOverScreen;
    std::weak_ptr<GameObject> gameOverText;
};

#endif
