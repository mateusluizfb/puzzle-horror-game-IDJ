#ifndef COMPOSITE_TILE_RENDERER_H
#define COMPOSITE_TILE_RENDERER_H

#include "Component.h"
#include "TileSet.h"
#include "Vec2.h"
#include <vector>

struct TileSprite {
  unsigned gid;
  Vec2 offset;
};

class CompositeTileRenderer : public Component {
private:
  TileSet tileSet;
  std::vector<TileSprite> tiles;
  Vec2 scale;

public:
  CompositeTileRenderer(GameObject& associated, const std::string& tileSetFile, int tileWidth, int tileHeight, Vec2 scale);
  
  void AddTile(unsigned gid, Vec2 offset);
  
  void Render() override;
  void Update(float dt) override;
};

#endif
