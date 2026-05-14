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

  void RenderTile(unsigned index, float x, float y, float scale = 1.0f);
  int GetTileWidth();
  int GetTileHeight();
};

#endif
