#include <gtest/gtest.h>
#include "Game.h"
#include "Log.h"
#include "TileSet.h"

TEST(TileSetTest, Constructor)
{
  Game *game = &Game::GetInstance("Test Game", 800, 600);

  Log::debug(" --- TileSetTest Logs ----");

  TileSet *tileSet = new TileSet(64, 64, "game/assets/img/Tileset.png");

  EXPECT_EQ(tileSet->GetTileWidth(), 64);
  EXPECT_EQ(tileSet->GetTileHeight(), 64);

  EXPECT_NO_THROW(tileSet->RenderTile(0, 0, 0));

  delete tileSet;
  delete game;
}

TEST(TileSetTest, TileScaling)
{
  Game *game = &Game::GetInstance("Test Game", 800, 600);

  Log::debug(" --- TileSetTest Logs ----");

  TileSet *tileSet = new TileSet(16, 16, "game/assets/img/Tileset.png");
  float scale = 2.0f;

  EXPECT_NO_THROW(tileSet->RenderTile(0, 0, 0, scale));

  delete tileSet;
  delete game;
}

TEST(TileSetTest, InvalidIndex)
{
  Game *game = &Game::GetInstance("Test Game", 800, 600);

  Log::debug(" --- TileSetTest Logs ----");

  TileSet *tileSet = new TileSet(64, 64, "game/assets/img/Tileset.png");

  unsigned invalidIndex = 1000;

  EXPECT_THROW(tileSet->RenderTile(invalidIndex, 0, 0), std::runtime_error);

  delete tileSet;
  delete game;
}
