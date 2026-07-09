#include "Log.h"
#include "Animator.h"
#include "Animation.h"
#include "GameObject.h"
#include "SpriteRenderer.h"
#include "Timer.h"

Animator::Animator(GameObject &associated)
    : Component(associated),
      frameStart(0),
      frameEnd(0),
      frameTime(0),
      currentFrame(0),
      timeElapsed(0),
      hitTimer(Timer()),
      deathTimer(Timer()),
      current("")
      {}

void Animator::Update(float dt) {
  if(frameTime == 0) return;

  timeElapsed += dt;

  if (timeElapsed < frameTime) return; // Frame didn't change yet

  currentFrame++;
  timeElapsed -= timeElapsed;

  if (currentFrame > frameEnd)
  {
    currentFrame = frameStart;
  }
  else
  {
    SpriteRenderer *spriteRenderer = associated.GetComponent<SpriteRenderer>();

    if (spriteRenderer != nullptr)
    {
      spriteRenderer->SetFrame(currentFrame);
    }
    else
    {
      throw std::runtime_error("Animator::Update(): No SpriteRenderer component found in associated GameObject.");
    }
  }
}

void Animator::Render() {
}

void Animator::SetAnimation(std::string name) {
  if (name == current) return;
  
  auto it = animations.find(name);
  
  if (it == animations.end()) {
    throw std::runtime_error("Animator::SetAnimation(): Animation '" + name + "' not found.");
  }

  Animation animation = it->second;

  frameStart = animation.frameStart;
  frameEnd = animation.frameEnd;
  frameTime = animation.frameTime;
  currentFrame = animation.frameStart;

  timeElapsed = 0;

  SpriteRenderer *spriteRenderer = associated.GetComponent<SpriteRenderer>();

  if (spriteRenderer != nullptr)
  {
    spriteRenderer->SetFrame(currentFrame, spriteRenderer->GetFlip());
  }
  else
  {
    throw std::runtime_error("Animator::Update(): No SpriteRenderer component found in associated GameObject.");
  }

  current = name;
}

void Animator::AddAnimation(std::string name, Animation anim) {
  if (animations.find(name) == animations.end())
  {
    animations.emplace(name, anim);
  }
}

int Animator::GetCurrentFrame() {
  return currentFrame;
}

std::string Animator::GetCurrent() {
  return current;
}
