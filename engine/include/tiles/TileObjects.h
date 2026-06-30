#ifndef TILEOBJECTS_H
#define TILEOBJECTS_H

#include <string>
#include <vector>
#include <map>
#include <functional>
#include "State.h"
#include "TileObject.h"
#include "Component.h"
#include "CompositeTileRenderer.h"
#include "Rect.h"
#include "Collider.h"

struct TileSetInfo {
  int firstgid;
  int tileCount;
  int tileWidth;
  int tileHeight;
  std::string imageFile; // resolved path to the tileset image
};

class TileObjects {
private:
  std::vector<TileObjectData> objects;

  // Tileset info parsed from the TMX/TSX
  std::string tmxFile;
  std::string tileSetFile;
  int tileWidth;
  int tileHeight;
  Vec2 scale;
  std::vector<TileSetInfo> tilesets;

  std::map<std::string, std::function<Component*(GameObject&)>> componentFactories;
  std::vector<std::string> componentRegistrationOrder;
  std::vector<GameObject*> compositeColliderObjects;

  void LoadTmx(const std::string& file);
  void SpawnObject(State& state, const TileObjectData& data);
  void MergeCompositeColliders(State& state);

public:
  TileObjects(const std::string& file, const std::string& tileSetFile, Vec2 scale);

  void RegisterComponent(const std::string& name, std::function<Component*(GameObject&)> factory);
  const std::vector<TileObjectData>& GetObjects() const;
  void Load(State &state);
};

#endif
