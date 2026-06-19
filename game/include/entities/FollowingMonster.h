#ifndef FOLLOWING_MONSTER_H
#define FOLLOWING_MONSTER_H

#include "Component.h"
#include "GameObject.h"
#include "Vec2.h"
#include "Timer.h"

class FollowingMonster : public Component
{
public:
    FollowingMonster(GameObject &associated);
    ~FollowingMonster();

    void Start() override;
    void Update(float dt) override;
    void Render() override;

private:
    float linearSpeed = 150.0f;
    Vec2 speed;
};

#endif