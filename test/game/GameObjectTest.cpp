#include <gtest/gtest.h>
#include "Game.h"
#include "Component.h"
#include "GameObject.h"
#include "State.h"
#include "StageState.h"
#include "SpriteRenderer.h"
#include "Log.h"

class MockComponent : public Component
{
public:
  MockComponent(GameObject &go) : Component(go) {}
  void Update(float) override { Log::info("MockComponent - Update called"); }
  void Render() override { Log::info("MockComponent - Render called"); }
};

TEST(GameTest, InitGame)
{
  Game *game = &Game::GetInstance("Test Game", 800, 600);

  delete game;
}

TEST(GameObjectTest, AddObjectAndComponent)
{
  // Needed so it inits the renderer used by SpriteRenderer's Sprite
  Game *game = &Game::GetInstance("Test Game", 800, 600);

  Log::debug(" --- AddObjectAndComponent Logs ----");

  State *state = new StageState();
  game->StateStackPush(new StageState());

  GameObject *obj = new GameObject();
  SpriteRenderer *spriteRenderer = new SpriteRenderer(*obj, "game/assets/img/black_solid_color.png");

  obj->AddComponent(spriteRenderer);
  state->AddObject(obj);

  MockComponent *comp = new MockComponent(*obj);
  obj->AddComponent(comp);

  // Should not be dead initially
  EXPECT_FALSE(obj->IsDead());

  // Update and Render should not crash
  EXPECT_NO_THROW(state->Update(0.0));
  EXPECT_NO_THROW(state->Render());

  // Request delete and check
  obj->RequestDelete();
  EXPECT_TRUE(obj->IsDead());

  // Update and Render after removal
  EXPECT_NO_THROW(state->Update(0.0));
  EXPECT_NO_THROW(state->Render());

  delete state;
  delete game;
}

TEST(GameObjectTest, Start) {
  GameObject *obj = new GameObject();
  MockComponent *comp = new MockComponent(*obj);

  obj->AddComponent(comp);

  EXPECT_NO_THROW(obj->Start());
}


TEST(GameObjectTest, NotifyCollisionCallsComponent)
{
  class CollisionMockComponent : public Component
  {
  public:
    bool collisionNotified = false;
    CollisionMockComponent(GameObject &go) : Component(go) {}
    void NotifyCollision(GameObject &) override { collisionNotified = true; }
    void Update(float) override {} // Provide empty implementation
    void Render() override {}      // Provide empty implementation
  };

  GameObject objA, objB;
  CollisionMockComponent *comp = new CollisionMockComponent(objA);
  objA.AddComponent(comp);

  objA.NotifyCollision(objB);

  EXPECT_TRUE(comp->collisionNotified);
}
