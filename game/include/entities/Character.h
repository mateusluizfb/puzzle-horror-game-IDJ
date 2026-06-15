#ifndef CHARACTER_H
#define CHARACTER_H

#include <queue>
#include <memory>
#include <string>
#include "Component.h"
#include "GameData.h"
#include "GameObject.h"
#include "Timer.h"
#include "Vec2.h"
#include "Sound.h"

#ifdef DEBUG
  #define IMMORTAL true
#else
  #define IMMORTAL false
#endif

enum CommandType
{
  MOVE,
};

class Character : public Component
{
public:
  class Command
  {
  public:
    Command(CommandType type, float x, float y);
    CommandType type;
    Vec2 pos;
  };

  Character* player;
  Sound hitSound;
  Sound deathSound;
  bool hit = false;

  Character(GameObject &associated, std::string sprite);
  ~Character();

  void Start() override;
  void Update(float dt) override;
  void Render() override;
  void Issue(Command command);
  std::queue<Command> GetTaskQueue();
  Vec2 GetSpeed();
  float GetLinearSpeed();
  void SetLinearSpeed(float linearSpeed);
  int GetHp();
  void NotifyCollision(GameObject &other) override;
  Vec2 GetCenterPosition();
  void TakeDamage();

private:
  std::queue<Command> taskQueue;
  Vec2 speed;
  float linearSpeed;
  int hp;
  Vec2 collisionNormal;

public:
  void SetCollisionNormal(Vec2 normal) { collisionNormal = normal; }
  Vec2 GetCollisionNormal() { return collisionNormal; }

private:
  Timer deathTimer;
  bool isInvulnerable;
  Timer invulnerabilityTimer;
};

#endif
