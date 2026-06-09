#include <gtest/gtest.h>
#include <SDL.h>
#include <SDL_image.h>
#include "Resources.h"
#include "Game.h"
#include "Log.h"

TEST(ResourcesTest, GetImage_CachesTexture) {
  Game *game = &Game::GetInstance("Test Game", 800, 600);

  Log::debug(" --- ResourcesTest Logs ----");

  std::string file = "game/assets/img/black_solid_color.png";

  SDL_Texture* tex1 = Resources::GetImage(file);
  SDL_Texture* tex2 = Resources::GetImage(file);

  EXPECT_NE(tex1, nullptr);
  EXPECT_EQ(tex1, tex2);

  Resources::ClearImages();
  delete game;
}

TEST(ResourcesTest, ClearImages_DestroysTextures) {
  Game *game = &Game::GetInstance("Test Game", 800, 600);

  Log::debug(" --- ResourcesTest Logs ----");

  std::string file = "game/assets/img/black_solid_color.png";

  SDL_Texture* tex1 = Resources::GetImage(file);
  EXPECT_NE(tex1, nullptr);

  Resources::ClearImages();

  SDL_Texture* tex2 = Resources::GetImage(file);
  EXPECT_NE(tex2, nullptr);

  Resources::ClearImages();
  delete game;
}
