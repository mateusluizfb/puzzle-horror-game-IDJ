#include "CompositeTileRenderer.h"
#include "../core/GameObject.h"

CompositeTileRenderer::CompositeTileRenderer(GameObject& associated, const std::string& tileSetFile, int tileWidth, int tileHeight, Vec2 scale)
  : Component(associated),
    tileSet(tileWidth, tileHeight, tileSetFile),
    scale(scale)
{}

void CompositeTileRenderer::AddTile(unsigned gid, Vec2 offset) {
  tiles.push_back({gid, offset});
}

void CompositeTileRenderer::Render() {
  for (const auto& tile : tiles) {
    unsigned index = tile.gid - 1;
    tileSet.RenderTile(
      index,
      associated.box.x + tile.offset.x,
      associated.box.y + tile.offset.y,
      scale
    );
  }
}

void CompositeTileRenderer::Update(float /*dt*/) {
  // Nothing to update
}
