#include <gtest/gtest.h>
#include "Log.h"
#include "Game.h"
#include "Character.h"
#include "Animator.h"
#include "Bullet.h"
#include "StageState.h"

TEST(CharacterTest, Start)
{
  Game *game = &Game::GetInstance("Test Game", 800, 600);
  game->StateStackPush(new StageState());

  Log::debug(" --- CharacterTest Logs ----");

  GameObject go;
  Character character(go, "game/assets/img/Player.png");
  
  State& previousState = game->GetCurrentState();
  int previousObjectsSize = previousState.GetObjectArray().size();
  
  EXPECT_NO_THROW(character.Start());

  State& currentState = game->GetCurrentState();
  int currentObjectsSize = currentState.GetObjectArray().size();

  EXPECT_EQ(currentObjectsSize, previousObjectsSize + 1);

  delete game;
}

TEST(CharacterTest, Issue) {
  Game *game = &Game::GetInstance("Test Game", 800, 600);
  game->StateStackPush(new StageState());

  Log::debug(" --- CharacterTest Logs ----");

  GameObject go;
  Character character(go, "game/assets/img/Player.png");
  Character::Command command(CommandType::MOVE, 0, 0);

  std::queue<Character::Command> taskQueue = character.GetTaskQueue();

  EXPECT_EQ(taskQueue.size(), 0);
  
  EXPECT_NO_THROW(character.Issue(command));
  
  taskQueue = character.GetTaskQueue();
  
  EXPECT_EQ(taskQueue.size(), 1);
  
  delete game;
}

TEST(CharacterTest, UpdateIdle) {
  Game *game = &Game::GetInstance("Test Game", 800, 600);
  game->StateStackPush(new StageState());

  Log::debug(" --- CharacterTest Logs ----");

  GameObject go;
  Character character(go, "game/assets/img/Player.png");

  EXPECT_NO_THROW(character.Update(0));

  Animator* animator = go.GetComponent<Animator>();

  ASSERT_TRUE(animator->GetCurrent() == "idle");
  EXPECT_EQ(character.GetSpeed().x, 0);
  EXPECT_EQ(character.GetSpeed().y, 0);

  delete game;
}

TEST(CharacterTest, UpdateShoot)
{
  Game *game = &Game::GetInstance("Test Game", 800, 600);
  game->StateStackPush(new StageState());
  State &currentState = game->GetCurrentState();
  std::weak_ptr<GameObject> player = currentState.GetObjectByTag("player");
  Character* character = player.lock()->GetComponent<Character>();

  Log::debug(" --- CharacterTest Logs ----");

  Character::Command command(CommandType::SHOOT, 0, 0);

  character->Issue(command);

  EXPECT_NO_THROW(character->Update(0));

  delete game;
}

TEST(CharacterTest, UpdateMove)
{
  Game *game = &Game::GetInstance("Test Game", 800, 600);
  game->StateStackPush(new StageState());

  Log::debug(" --- CharacterTest Logs ----");

  GameObject* go = new GameObject();
  Character *character = new Character(*go, "game/assets/img/Player.png");
  go->AddComponent(character);
  Character::Command command(CommandType::MOVE, 1, 1);

  character->Start();
  character->Issue(command);

  EXPECT_NO_THROW(character->Update(1));

  Animator *animator = go->GetComponent<Animator>();

  ASSERT_TRUE(animator->GetCurrent() == "walking");
  EXPECT_NEAR(character->GetSpeed().x, 70.7106769, 1e-5);
  EXPECT_NEAR(character->GetSpeed().y, 70.7106769, 1e-5);
  EXPECT_NEAR(go->box.x, 70.7106769, 1e-5);
  EXPECT_NEAR(go->box.y, 70.7106769, 1e-5);

  delete go;
  delete game;
}

TEST(CharacterTest, NotifyCollision) {
  Game *game = &Game::GetInstance("Test Game", 800, 600);
  game->StateStackPush(new StageState());

  Log::debug(" --- CharacterTest Logs ----");

  GameObject *go = new GameObject();
  Character *character = new Character(*go, "game/assets/img/Player.png");
  go->AddComponent(character);
  character->Start();

  // Simulate collision (should not reduce hp yet)
  GameObject *other = new GameObject();
  character->NotifyCollision(*other);
  EXPECT_EQ(character->GetHp(), 100);
}

TEST(CharacterTest, NotifyCollisionWithBullet)
{
  Game *game = &Game::GetInstance("Test Game", 800, 600);
  game->StateStackPush(new StageState());

  Log::debug(" --- CharacterTest NotifyCollisionWithBullet Logs ----");

  GameObject go;
  Character character(go, "game/assets/img/Player.png");
  character.Start();

  GameObject bulletGO;
  Bullet* bullet = new Bullet(bulletGO, 0, 0, 30, 100, true); // Bullet that targets player
  bulletGO.AddComponent(bullet);

  EXPECT_EQ(character.GetHp(), 100);

  // Force hitTimer above threshold and collide with bullet
  Animator* animator = go.GetComponent<Animator>();
  animator->hitTimer.Restart();
  animator->hitTimer.Update(2.1f); // Force timer above threshold
  character.NotifyCollision(bulletGO);

  // After valid collision with bullet, hp should be reduced
  EXPECT_EQ(character.GetHp(), 70);

  delete game;
}
