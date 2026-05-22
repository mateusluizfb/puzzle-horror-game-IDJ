#include <gtest/gtest.h>
#include "Game.h"
#include "GameObject.h"
#include "State.h"
#include "StageState.h"
#include "TileObject.h"
#include "TileObjects.h"
#include "Log.h"

static const std::string TMX_PATH      = "game/assets/map/tilemap.tmx";
static const std::string TILESET_IMG   = "game/assets/img/Tileset.png";
static constexpr int     TILE_W        = 64;
static constexpr int     TILE_H        = 64;

// ── TileObject component ──────────────────────────────────────────────────────

TEST(TileObjectTest, SetsBoxAndTagFromData)
{
  Game* game = &Game::GetInstance("Test Game", 800, 600);

  TileObjectData data;
  data.id     = 1;
  data.name   = "box";
  data.gid    = 6;
  data.x      = 100.f;
  data.y      = 200.f;
  data.width  = 64.f;
  data.height = 64.f;

  GameObject* go = new GameObject();
  TileObject* obj = new TileObject(*go, data, TILESET_IMG, TILE_W, TILE_H, Vec2(1.0f, 1.0f));
  go->AddComponent(obj);

  EXPECT_NEAR(go->box.x, 100.f, 0.01f);
  EXPECT_NEAR(go->box.y, 200.f, 0.01f);
  EXPECT_NEAR(go->box.w,  64.f, 0.01f);
  EXPECT_NEAR(go->box.h,  64.f, 0.01f);
  EXPECT_EQ(go->tag, "box");

  delete go;
  delete game;
}

TEST(TileObjectTest, GetDataReturnsCorrectData)
{
  Game* game = &Game::GetInstance("Test Game", 800, 600);

  TileObjectData data;
  data.id     = 6;
  data.name   = "box";
  data.gid    = 6;
  data.x      = 705.f;
  data.y      = 833.f;
  data.width  = 64.f;
  data.height = 64.f;
  data.properties["collider"] = "true";
  data.properties["pushable"] = "true";

  GameObject* go = new GameObject();
  TileObject* obj = new TileObject(*go, data, TILESET_IMG, TILE_W, TILE_H, Vec2(1.0f, 1.0f));
  go->AddComponent(obj);

  const TileObjectData& got = obj->GetData();
  EXPECT_EQ(got.name, "box");
  EXPECT_EQ(got.gid, 6);
  EXPECT_EQ(got.properties.at("collider"), "true");

  delete go;
  delete game;
}

TEST(TileObjectTest, RenderDoesNotThrow)
{
  Game* game = &Game::GetInstance("Test Game", 800, 600);

  TileObjectData data;
  data.id     = 1;
  data.name   = "box";
  data.gid    = 6;
  data.x      = 0.f;
  data.y      = 0.f;
  data.width  = 64.f;
  data.height = 64.f;

  GameObject* go = new GameObject();
  TileObject* obj = new TileObject(*go, data, TILESET_IMG, TILE_W, TILE_H, Vec2(1.0f, 1.0f));
  go->AddComponent(obj);

  EXPECT_NO_THROW(obj->Render());

  delete go;
  delete game;
}

TEST(TileObjectTest, UpdateDoesNotThrow)
{
  Game* game = &Game::GetInstance("Test Game", 800, 600);

  TileObjectData data;
  data.id = 1; data.name = "box"; data.gid = 1;
  data.x = 0; data.y = 0; data.width = 64; data.height = 64;

  GameObject* go = new GameObject();
  go->AddComponent(new TileObject(*go, data, TILESET_IMG, TILE_W, TILE_H, Vec2(1.0f, 1.0f)));

  EXPECT_NO_THROW(go->Update(0.016f));

  delete go;
  delete game;
}

// ── TileObjects ─────────────────────────────────────────────────────────

TEST(TileObjectsLoaderTest, LoadsObjectsFromTmx)
{
  Game* game = &Game::GetInstance("Test Game", 800, 600);

  Log::debug(" --- TileObjectsLoaderTest::LoadsObjectsFromTmx ---");

  State* state = new StageState();
  game->StateStackPush(new StageState());

  TileObjects loader(TMX_PATH, TILESET_IMG, Vec2(1.0f, 1.0f));
  loader.Load(*state);

  std::weak_ptr<GameObject> found = state->GetObjectByTag("box");
  ASSERT_FALSE(found.expired());

  TileObject* comp = found.lock()->GetComponent<TileObject>();
  EXPECT_NE(comp, nullptr);

  delete state;
  delete game;
}

TEST(TileObjectsLoaderTest, InvalidFileThrows)
{
  Game* game = &Game::GetInstance("Test Game", 800, 600);

  State* state = new StageState();
  game->StateStackPush(new StageState());

  EXPECT_THROW(
    TileObjects("nonexistent_file.tmx", TILESET_IMG, Vec2(1.0f, 1.0f)).Load(*state),
    std::runtime_error
  );

  delete state;
  delete game;
}
