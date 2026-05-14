#include "TileObject.h"
#include "Camera.h"
#include "SpriteRenderer.h"

TileObject::TileObject(GameObject& associated,
                       const TileObjectData& data,
                       const std::string& tileSetFile,
                       int tileWidth,
                       int tileHeight)
  : Component(associated)
  , data(data)
  , tileSet(tileWidth, tileHeight, tileSetFile)
{
  associated.box.x = data.x;
  associated.box.y = data.y;
  associated.box.w = data.width;
  associated.box.h = data.height;
  associated.tag   = data.name;
}

void TileObject::Render() {
  if (data.gid <= 0) return;

  // TMX gids are 1-based; convert to 0-based tile index.
  unsigned index = static_cast<unsigned>(data.gid - 1);
  tileSet.RenderTile(index, associated.box.x, associated.box.y, 1.0f);
}

void TileObject::Update(float /*dt*/) {
  // Nothing yet.
}

const TileObjectData& TileObject::GetData() const {
  return data;
}
