#pragma once
#include "Component.h"
#include "Timer.h"

class DarkZone : public Component {
private:
    Timer damageTimer;

public:
    DarkZone(GameObject& associated);
    void Update(float dt) override;
    void Render() override;
    void NotifyCollision(GameObject& other) override;
};
