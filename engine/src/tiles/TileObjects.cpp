#include <stdexcept>
#include <string>
#include <set>

#include "Log.h"
#include "TileObjects.h"
#include "TileObject.h"
#include "CompositeTileRenderer.h"
#include "GameObject.h"
#include "XmlParser.h"

// Returns the directory portion of a file path (including the trailing slash).
static std::string DirOf(const std::string& path) {
  size_t sep = path.find_last_of("/\\");
  return (sep == std::string::npos) ? "" : path.substr(0, sep + 1);
}

TileObjects::TileObjects(const std::string &tmxFile, const std::string &tileSetFile, Vec2 scale)
    : tmxFile(tmxFile), tileSetFile(tileSetFile), tileWidth(0), tileHeight(0), scale(scale)
{}

void TileObjects::RegisterComponent(const std::string& name, std::function<Component*(GameObject&)> factory) {
  componentFactories[name] = factory;
  componentRegistrationOrder.push_back(name);
}

void TileObjects::Load(State &state)
{
  Log::info("TILE_OBJECTS - Loading tile objects from " + tmxFile);
  LoadTmx(tmxFile);

  Log::info("TILE_OBJECTS - Spawning " + std::to_string(objects.size()) + " objects");
  for (const auto& data : objects) {
    SpawnObject(state, data);
  }
  MergeCompositeColliders(state);
}

void TileObjects::LoadTmx(const std::string& file) {
  XmlNode root = XmlParser::ParseFile(file);

  if (root.tag != "map") {
    throw std::runtime_error("TileObjects::LoadTmx: Root element is not <map> in " + file);
  }

  // --- Parse ALL tilesets referenced by the TMX ---
  std::string tmxDir = DirOf(file);

  for (const XmlNode* tilesetRef : root.findChildren("tileset")) {
    TileSetInfo info;
    info.firstgid = std::stoi(tilesetRef->getAttribute("firstgid", "1"));

    std::string tsxSource = tilesetRef->getAttribute("source");
    if (tsxSource.empty()) {
      // Inline tileset (no external TSX). Read tile dimensions directly.
      info.tileWidth  = std::stoi(tilesetRef->getAttribute("tilewidth",  "0"));
      info.tileHeight = std::stoi(tilesetRef->getAttribute("tileheight", "0"));
      info.tileCount  = std::stoi(tilesetRef->getAttribute("tilecount",  "0"));
      const XmlNode* image = tilesetRef->findChild("image");
      if (image) {
        std::string imgSource = image->getAttribute("source");
        info.imageFile = tmxDir + imgSource;
      }
    } else {
      std::string tsxPath = tmxDir + tsxSource;
      XmlNode tsx = XmlParser::ParseFile(tsxPath);

      info.tileWidth  = std::stoi(tsx.getAttribute("tilewidth",  "0"));
      info.tileHeight = std::stoi(tsx.getAttribute("tileheight", "0"));
      info.tileCount  = std::stoi(tsx.getAttribute("tilecount",  "0"));

      const XmlNode* image = tsx.findChild("image");
      if (image) {
        std::string imgSource = image->getAttribute("source");
        // The image path in the TSX is relative to the TSX file's directory.
        std::string tsxDir = DirOf(tsxPath);
        info.imageFile = tsxDir + imgSource;
      }
    }

    tilesets.push_back(info);
  }

  if (tilesets.empty()) {
    throw std::runtime_error("TileObjects::LoadTmx: No <tileset> found in " + file);
  }

  // Keep tileWidth/tileHeight for backwards compatibility (use the first tileset).
  tileWidth  = tilesets.front().tileWidth;
  tileHeight = tilesets.front().tileHeight;

  // --- Parse object group ---
  const XmlNode* objectgroup = root.findChild("objectgroup");
  if (!objectgroup) {
    Log::warning("TileObjects::LoadTmx: No <objectgroup> found in " + file);
    return;
  }

  for (const XmlNode* obj : objectgroup->findChildren("object")) {
    TileObjectData data;
    data.id     = std::stoi(obj->getAttribute("id",     "0"));
    data.name   = obj->getAttribute("name");
    // evitar overflow e suporta rotacao de sprite
    unsigned long long raw_gid = std::stoull(obj->getAttribute("gid", "0"));
    bool flipH = (raw_gid & 0x80000000) != 0; // Bit 32
    bool flipV = (raw_gid & 0x40000000) != 0; // Bit 31
    bool flipD = (raw_gid & 0x20000000) != 0; // Bit 30

    data.gid = (int)(raw_gid & 0x1FFFFFFF); // Limpa as 3 flags mantendo a ID pura

    data.angle = 0.0f;
    data.flipH = false;
    data.flipV = false;

    // Traduz os bits diagonais malucos para angulos reais da sua Engine
    if (flipD) {
        if (flipH && !flipV) { data.angle = 90.0f; } 
        else if (!flipH && flipV) { data.angle = 270.0f; } // -90 graus
        else if (flipH && flipV) { data.angle = 90.0f; data.flipV = true; }
        else if (!flipH && !flipV) { data.angle = 270.0f; data.flipV = true; }
    } else {
        if (flipH && flipV) { data.angle = 180.0f; }
        else { data.flipH = flipH; data.flipV = flipV; }
    }

    // Soma caso o Level Designer use a ferramenta livre de girar (atributo 'rotation' nativo do XML)
    data.angle += std::stof(obj->getAttribute("rotation", "0"));

    data.x      = std::stof(obj->getAttribute("x",      "0"));
    data.y      = std::stof(obj->getAttribute("y",      "0"));
    data.width  = std::stof(obj->getAttribute("width",  "0"));
    data.height = std::stof(obj->getAttribute("height", "0"));

    // Resolve which tileset this object's tile belongs to and compute local index info.
    data.firstgid = 1;
    data.tileSetFile = tileSetFile;
    if (data.gid > 0) {
      // Find the tileset whose gid range contains this gid (last tileset with firstgid <= gid).
      const TileSetInfo* matched = nullptr;
      for (const auto& ts : tilesets) {
        if (ts.firstgid <= data.gid) {
          if (!matched || ts.firstgid > matched->firstgid) {
            matched = &ts;
          }
        }
      }
      if (matched) {
        data.firstgid = matched->firstgid;
        data.tileSetFile = matched->imageFile;
      } else {
        Log::warning("TileObjects::LoadTmx: No tileset found for gid=" +
                     std::to_string(data.gid) + " in " + file);
      }
    }

    const XmlNode* props = obj->findChild("properties");
    if (props) {
      for (const XmlNode* prop : props->findChildren("property")) {
        std::string name  = prop->getAttribute("name");
        std::string value = prop->getAttribute("value");
        if (!name.empty()) {
          data.properties[name] = value;
        }
      }
    }

    objects.push_back(data);
  }
}

void TileObjects::SpawnObject(State& state, const TileObjectData& data) {
  Log::debug("TILE_OBJECTS - Spawning object id=" + std::to_string(data.id));

  bool hasCollider = data.properties.count("collider") && data.properties.at("collider") == "true";
  bool hasComposite = data.properties.count("composite_collider") && data.properties.at("composite_collider") == "true";
  if (hasCollider && hasComposite) {
    Log::warning("TILE_OBJECTS - Object id=" + std::to_string(data.id) + " has both 'collider' and 'composite_collider'. 'composite_collider' will take precedence.");
  }

  GameObject *go = new GameObject();
  go->angleDeg = data.angle;
  go->AddComponent(new TileObject(*go, data, data.tileSetFile, tileWidth, tileHeight, scale));

  for (const std::string& key : componentRegistrationOrder) {
    auto propIt = data.properties.find(key);
    if (propIt == data.properties.end() || propIt->second != "true") continue;
    
    if (key == "collider" && hasComposite) continue;

    auto it = componentFactories.find(key);
    if (it != componentFactories.end()) {
      Component* component = it->second(*go);
      go->AddComponent(component);
    }
  }

  if (hasComposite) {
    compositeColliderObjects.push_back(go);
  }

  state.AddObject(go);
}

const std::vector<TileObjectData>& TileObjects::GetObjects() const {
  return objects;
}

void TileObjects::MergeCompositeColliders(State& state) {
  if (compositeColliderObjects.empty()) return;

  size_t n = compositeColliderObjects.size();
  std::vector<int> parent(n);
  for (size_t i = 0; i < n; ++i) parent[i] = (int)i;

  auto find = [&](int i) {
    int root = i;
    while (parent[root] != root) root = parent[root];
    while (parent[i] != root) {
      int next = parent[i];
      parent[i] = root;
      i = next;
    }
    return root;
  };

  auto unite = [&](int i, int j) {
    int rootI = find(i);
    int rootJ = find(j);
    if (rootI != rootJ) parent[rootI] = rootJ;
  };

  for (size_t i = 0; i < n; ++i) {
    for (size_t j = i + 1; j < n; ++j) {
      if (Rect::Overlaps(compositeColliderObjects[i]->box, compositeColliderObjects[j]->box)) {
        unite((int)i, (int)j);
      }
    }
  }

  std::map<int, std::vector<size_t>> groups;
  for (size_t i = 0; i < n; ++i) {
    groups[find((int)i)].push_back(i);
  }

  int mergedCount = 0;
  for (auto it = groups.begin(); it != groups.end(); ++it) {
    const std::vector<size_t>& members = it->second;
    if (members.size() <= 1) continue;

    Rect mergedBox = compositeColliderObjects[members[0]]->box;
    for (size_t i = 1; i < members.size(); ++i) {
      mergedBox = Rect::Merge(mergedBox, compositeColliderObjects[members[i]]->box);
    }

    GameObject* primary = compositeColliderObjects[members[0]];
    primary->box = mergedBox;
    Collider* col = primary->GetComponent<Collider>();
    if (col) {
      col->GetBox() = mergedBox;
      col->SetComposite(true);
    }

    CompositeTileRenderer* renderer = new CompositeTileRenderer(*primary, tileSetFile, tileWidth, tileHeight, scale);
    
    TileObject* primaryTile = primary->GetComponent<TileObject>();
    if (primaryTile) {
      const TileObjectData& data = primaryTile->GetData();
      renderer->AddTile(data.gid, data.firstgid, Vec2(0, 0));
    }
    primary->AddComponent(renderer);

    std::set<std::string> componentsOnPrimary;
    for (const std::string& key : componentRegistrationOrder) {
      auto propIt = primaryTile->GetData().properties.find(key);
      if (propIt != primaryTile->GetData().properties.end() && propIt->second == "true") {
        componentsOnPrimary.insert(key);
      }
    }

    for (size_t i = 1; i < members.size(); ++i) {
      GameObject* go = compositeColliderObjects[members[i]];
      TileObject* tile = go->GetComponent<TileObject>();
      if (tile) {
        const TileObjectData& data = tile->GetData();
        Vec2 offset(
          go->box.x - primary->box.x,
          go->box.y - primary->box.y
        );
        renderer->AddTile(data.gid, data.firstgid, offset);
        
        for (const std::string& key : componentRegistrationOrder) {
          auto propIt = data.properties.find(key);
          if (propIt == data.properties.end() || propIt->second != "true") continue;
          
          if (componentsOnPrimary.find(key) == componentsOnPrimary.end()) {
            auto it = componentFactories.find(key);
            if (it != componentFactories.end()) {
              Component* component = it->second(*primary);
              primary->AddComponent(component);
              componentsOnPrimary.insert(key);
            }
          }
        }
      }

      go->RequestDelete();
      mergedCount++;
    }
  }

  Log::info("TILE_OBJECTS - Merged " + std::to_string(mergedCount) + " composite colliders into " + std::to_string(groups.size()) + " groups");
}
