#include <memory>
#include "State.h"

State::State()
  : popRequested(false),
    quitRequested(false),
    isOverlay(false),
    started(false) {}

State::~State() {
  for (auto &obj : objectArray)
  {
    obj->RequestDelete();
  }
}

std::weak_ptr<GameObject> State::AddObject(GameObject *go)
{
  std::shared_ptr<GameObject> go_shared_ptr = std::shared_ptr<GameObject>(go);
  objectArray.push_back(go_shared_ptr);

  if (started)
  {
    go->Start();
  }

  return go_shared_ptr;
}

std::weak_ptr<GameObject> State::GetObjectPtr(GameObject *go)
{
  for (size_t i = 0; i < objectArray.size(); i++)
  {
    if (objectArray[i].get() == go)
    {
      std::weak_ptr<GameObject> goWeakPtr = objectArray[i];

      return goWeakPtr;
    }
  }

  return std::weak_ptr<GameObject>();
}

std::weak_ptr<GameObject> State::GetObjectByTag(const std::string& tag)
{
  for (size_t i = 0; i < objectArray.size(); i++)
  {
    if (objectArray[i]->tag == tag)
    {
      return objectArray[i];
    }
  }

  return std::weak_ptr<GameObject>();
}

std::vector<std::shared_ptr<GameObject>> State::GetObjectArray()
{
  return objectArray;
}

bool State::QuitRequested()
{
  return quitRequested;
}

void State::RequestQuit()
{
  quitRequested = true;
}

void State::RequestPop()
{
  popRequested = true;
}

bool State::PopRequested()
{
  return popRequested;
}

void State::Pause() {}

void State::Resume() {}

void State::StartArray()
{
  for (size_t i = 0; i < objectArray.size(); i++)
  {
    objectArray[i]->Start();
  }
  started = true;
}

void State::UpdateArray(float dt)
{
  for (size_t i = 0; i < objectArray.size(); i++)
  {
    objectArray[i]->Update(dt);
  }
}

void State::UpdateDead()
{
  for (size_t i = 0; i < objectArray.size(); i++)
  {
    if (objectArray[i]->IsDead()) {
      objectArray.erase(objectArray.begin() + i);
    }
  }
}

void State::RenderArray()
{
  for (size_t i = 0; i < objectArray.size(); i++)
  {
    objectArray[i]->Render();
  }
}

void State::SetIsOverlay() {
  isOverlay = true;
}
