#ifndef TILEOBJECTS_H
#define TILEOBJECTS_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include "State.h"
#include "TileObject.h"
#include "Component.h"

class TileObjects {
private:
  std::vector<TileObjectData> objects;

  // Tileset info parsed from the TMX/TSX
  std::string tmxFile;
  std::string tileSetFile;
  int tileWidth;
  int tileHeight;
  Vec2 scale;

  std::map<std::string, std::function<Component*(GameObject&)>> componentFactories;
  std::vector<std::string> componentRegistrationOrder;

  void LoadTmx(const std::string& file);
  void SpawnObject(State& state, const TileObjectData& data);

public:
  TileObjects(const std::string& file, const std::string& tileSetFile, Vec2 scale);

  void RegisterComponent(const std::string& name, std::function<Component*(GameObject&)> factory);
  const std::vector<TileObjectData>& GetObjects() const;
  void Load(State &state);
};

#endif
