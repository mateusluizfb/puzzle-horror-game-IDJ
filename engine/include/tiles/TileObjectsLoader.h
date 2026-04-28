#ifndef TILEOBJECTS_H
#define TILEOBJECTS_H

#include <string>
#include <vector>
#include "State.h"
#include "TileObject.h"

class TileObjects {
private:
  std::vector<TileObjectData> objects;

  // Tileset info parsed from the TMX/TSX
  std::string tmxFile;
  std::string tileSetFile;
  int tileWidth;
  int tileHeight;

  void LoadTmx(const std::string& file);
  void SpawnObject(State& state, const TileObjectData& data);

public:
  TileObjects(const std::string& file, const std::string& tileSetFile);

  const std::vector<TileObjectData>& GetObjects() const;
  void Load(State &state);
};

#endif
