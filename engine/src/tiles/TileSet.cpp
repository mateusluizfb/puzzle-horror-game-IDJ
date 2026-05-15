#include <string>
#include "TileSet.h"
#include "Log.h"

TileSet::TileSet(int tileWidth, int tileHeight, std::string file)
: tileSet(file), tileWidth(tileWidth), tileHeight(tileHeight)
{
  Log::info("TILE_SET - Initing TileSet from: " + file);

  int cols = tileSet.GetWidth() / tileWidth;
  int rows = tileSet.GetHeight() / tileHeight;

  this->tileCount = cols * rows;
  tileSet.SetFrameCount(cols, rows);
};

void TileSet::RenderTile(unsigned index, float x, float y, Vec2 scale)
{
  if (index >= static_cast<unsigned>(tileSet.GetFrameCount()))
  {
    throw std::runtime_error("Tile index out of bounds: " + std::to_string(index) +
                 " (max: " + std::to_string(tileSet.GetFrameCount() - 1) + ")");
  }

  tileSet.SetFrame(index);
   tileSet.SetScale(scale.x, scale.y);
  tileSet.Render(x, y, tileWidth, tileHeight);
}

int TileSet::GetTileWidth() {
  return tileWidth;
}

int TileSet::GetTileHeight()
{
  return tileHeight;
}
