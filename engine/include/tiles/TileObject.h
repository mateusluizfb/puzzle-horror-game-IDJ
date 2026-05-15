#ifndef TILEOBJECT_H
#define TILEOBJECT_H

#include <string>
#include <map>
#include "Component.h"
#include "TileSet.h"

struct TileObjectData {
  int id;
  std::string name;
  int gid;               // 1-based global tile id from TMX
  float x;
  float y;
  float width;
  float height;
  std::map<std::string, std::string> properties;
};

class TileObject : public Component {
private:
  TileObjectData data;
  TileSet tileSet;
  Vec2 scale;

public:
  // tileSetFile  : path to the tileset image (e.g. "game/assets/img/Tileset.png")
  // tileWidth/Height : tile dimensions read from the TSX
  TileObject(GameObject& associated,
             const TileObjectData& data,
             const std::string& tileSetFile,
             int tileWidth,
             int tileHeight,
              Vec2 scale);

  void Render() override;
  void Update(float dt) override;

  const TileObjectData& GetData() const;
};

#endif
