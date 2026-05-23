#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "Component.h"
#include "Rect.h"
#include <vector>
#include <string>

class GameObject {
private:
  bool started;
  bool isDead;
  std::vector<Component *> components;

public:
  Rect box;
  double angleDeg;
  std::string tag;

  GameObject();
  ~GameObject();

  void Start();
  void Update(float dt);
  void Render();
  bool IsDead();
  void RequestDelete();
  void AddComponent(Component* cpt);
  void RemoveComponent(Component* cpt);
  float GetAngleRad();
  void NotifyCollision(GameObject& other);
  
  const std::vector<Component*>& GetComponents() const { return components; }

  template <typename T>
  T *GetComponent()
  {
    long unsigned int index;
    for (index = 0; index < components.size(); index++)
    {
      // T *component = dynamic_cast<T *>(components[index].get());
      T *component = dynamic_cast<T *>(components[index]);
      if (component != nullptr)
      {
        return component;
      }
    }
    return nullptr;
  }
};

#endif
