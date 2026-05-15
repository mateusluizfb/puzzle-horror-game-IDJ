#ifndef TILESET_H
#define TILESET_H

#include <string>
#include <Sprite.h>

class TileSet {
private:
  Sprite tileSet;
  int tileWidth;
  int tileHeight;
  int tileCount;

public:
  TileSet(int tileWidth, int tileHeight, std::string file);

  void RenderTile(unsigned index, float x, float y, Vec2 scale = Vec2(1.0f, 1.0f));
  int GetTileWidth();
  int GetTileHeight();
};

#endif
