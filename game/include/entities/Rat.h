#ifndef RAT_H
#define RAT_H

#include "Component.h"
#include "GameObject.h"
#include "Text.h"

class Rat : public Component
{
public:
    Rat(GameObject &associated, int x, int y);
    ~Rat();

    void Start() override;
    void Update(float dt) override;
    void Render() override;
    void NotifyCollision(GameObject &other) override;

private:
    Text *promptText = nullptr;
    bool isTouching = false;
    bool triggered = false;
    bool quizStarted = false;
};

#endif