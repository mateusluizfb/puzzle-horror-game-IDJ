#ifndef TILEMAP_H
#define TILEMAP_H

#include <vector>
#include <Component.h>

#include <memory>
#include <TileSet.h>
#include <XmlParser.h>

class TileMap : public Component {
private:
  std::vector<int> tileMatrix;
  std::unique_ptr<TileSet> tileSet;

  int mapWidth;
  int mapHeight;
  int mapDepth;
  bool isTmx;

  void LoadTxt(const std::string& file);
  void LoadTmx(const std::string& file);

public:
  Vec2 scale;

  TileMap(GameObject &associated, std::string file, TileSet *tileSet);

  void Load(std::string file);
  void SetTileSet(TileSet *tileSet);
  int& At(int x, int y, int z = 0);

  void RenderLayer();
  void RenderLayerTxt();
  void RenderLayerTmx();
  void Render() override;
  void Update(float dt) override;

  int GetWidth();
  int GetHeight();
  int GetDepth();
  std::vector<int> GetTileMatrix();
};

#endif
