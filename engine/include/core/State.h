#ifndef STATE_H
#define STATE_H

#include <vector>
#include <memory>
#include "GameObject.h"

class State {
public:
  State();
  virtual ~State();

  virtual void LoadAssets() = 0;
  virtual void Update(float dt) = 0;
  virtual void Render() = 0;

  virtual void Start() = 0;
  virtual void Pause();
  virtual void Resume();

  virtual std::weak_ptr<GameObject> AddObject(GameObject* go);
  virtual std::weak_ptr<GameObject> GetObjectPtr(GameObject* go);
  std::weak_ptr<GameObject> GetObjectByTag(const std::string& tag);
  std::vector<std::shared_ptr<GameObject>> GetObjectArray();
  
  bool PopRequested();
  bool QuitRequested();

  void RequestPop();
  void RequestQuit();

protected:
  bool popRequested;
  bool quitRequested;
  bool started;
  std::vector<std::shared_ptr<GameObject>> objectArray;

  void StartArray();
  void UpdateArray(float dt);
  void UpdateDead();
  void RenderArray();
};

#endif
