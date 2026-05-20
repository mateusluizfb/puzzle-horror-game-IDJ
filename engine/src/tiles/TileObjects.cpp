#include <stdexcept>
#include <string>

#include "Log.h"
#include "TileObjects.h"
#include "TileObject.h"
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

  // --- Parse tileset info from the external TSX file ---
  const XmlNode* tilesetRef = root.findChild("tileset");
  if (!tilesetRef) {
    throw std::runtime_error("TileObjects::LoadTmx: No <tileset> found in " + file);
  }

  std::string tsxSource = tilesetRef->getAttribute("source");
  if (tsxSource.empty()) {
    throw std::runtime_error("TileObjects::LoadTmx: <tileset> has no source attribute in " + file);
  }

  std::string tmxDir = DirOf(file);
  std::string tsxPath = tmxDir + tsxSource;

  XmlNode tsx = XmlParser::ParseFile(tsxPath);

  tileWidth  = std::stoi(tsx.getAttribute("tilewidth",  "0"));
  tileHeight = std::stoi(tsx.getAttribute("tileheight", "0"));

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
    data.gid    = std::stoi(obj->getAttribute("gid",    "0"));
    data.x      = std::stof(obj->getAttribute("x",      "0"));
    data.y      = std::stof(obj->getAttribute("y",      "0"));
    data.width  = std::stof(obj->getAttribute("width",  "0"));
    data.height = std::stof(obj->getAttribute("height", "0"));

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

  // Check for mutual exclusivity between 'collider' and 'composite_collider'
  bool hasCollider = data.properties.count("collider") && data.properties.at("collider") == "true";
  bool hasComposite = data.properties.count("composite_collider") && data.properties.at("composite_collider") == "true";
  if (hasCollider && hasComposite) {
    Log::warning("TILE_OBJECTS - Object id=" + std::to_string(data.id) + " has both 'collider' and 'composite_collider'. 'composite_collider' will take precedence.");
  }

  GameObject *go = new GameObject();
  go->AddComponent(new TileObject(*go, data, tileSetFile, tileWidth, tileHeight, scale));

  for (const std::string& key : componentRegistrationOrder) {
    auto propIt = data.properties.find(key);
    if (propIt == data.properties.end() || propIt->second != "true") continue;
    
    // Respect mutual exclusivity for the actual component spawning
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
      if (RectsOverlap(compositeColliderObjects[i]->box, compositeColliderObjects[j]->box)) {
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
      mergedBox = MergeRects(mergedBox, compositeColliderObjects[members[i]]->box);
    }

    GameObject* primary = compositeColliderObjects[members[0]];
    primary->box = mergedBox;
    Collider* col = primary->GetComponent<Collider>();
    if (col) {
      col->GetBox() = mergedBox;
      col->SetComposite(true);
    }

    for (size_t i = 1; i < members.size(); ++i) {
      GameObject* go = compositeColliderObjects[members[i]];
      Collider* col = go->GetComponent<Collider>();
      if (col) {
        go->RemoveComponent(col);
        delete col;
      }
      mergedCount++;
    }
  }

  Log::info("TILE_OBJECTS - Merged " + std::to_string(mergedCount) + " composite colliders into " + std::to_string(groups.size()) + " groups");
}
