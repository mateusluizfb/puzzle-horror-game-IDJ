#ifndef FOLLOWING_MONSTER_H
#define FOLLOWING_MONSTER_H

#include <cstdlib>
#include "Component.h"
#include "GameObject.h"
#include "Vec2.h"
#include "Timer.h"
#include "SpriteRenderer.h"

class Animator;

class FollowingMonster : public Component
{
public:
    FollowingMonster(GameObject &associated);
    ~FollowingMonster();

    void Start() override;
    void Update(float dt) override;
    void Render() override;

private:
    enum class State { Stage1, Flicker1, Stage2, Flicker2, Stage3 };

    void FollowPlayer(float dt, Animator *animator, bool checkProximity);

    float linearSpeed = 70.0f;
    Vec2 speed;
    State currentState = State::Stage1;
    Timer flickerTimer;
};

#endif
