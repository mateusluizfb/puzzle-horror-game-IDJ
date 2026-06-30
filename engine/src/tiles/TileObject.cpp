#include "TileObject.h"
#include "Camera.h"
#include "SpriteRenderer.h"

TileObject::TileObject(GameObject& associated,
                       const TileObjectData& data,
                       const std::string& tileSetFile,
                       int tileWidth,
                       int tileHeight,
                       Vec2 scale)
  : Component(associated)
  , data(data)
  , tileSet(tileWidth, tileHeight, tileSetFile)
  , scale(scale)
{
  associated.box.x = data.x * scale.x;
  associated.box.y = data.y * scale.y;
  associated.box.w = data.width * scale.x;
  associated.box.h = data.height * scale.y;
  associated.tag   = data.name;
}

void TileObject::Render() {
  if (hidden) return;
  if (data.gid <= 0) return;

  // TMX gids are global; convert to local 0-based index using the tileset's firstgid.
  unsigned index = static_cast<unsigned>(data.gid - data.firstgid);
  tileSet.RenderTileEx(
    index,
    associated.box.x,
    associated.box.y,
    associated.angleDeg,
    data.flipH,
    data.flipV,
    scale
  );
}

void TileObject::Update(float /*dt*/) {
  // Nothing yet.
}

const TileObjectData& TileObject::GetData() const {
  return data;
}

void TileObject::Show() {
  hidden = false;
}

void TileObject::Hide() {
  hidden = true;
}
