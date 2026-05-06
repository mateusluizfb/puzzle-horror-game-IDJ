#include <stdexcept>
#include <string>

#include "Log.h"
#include "TileObjectsLoader.h"
#include "TileObject.h"
#include "GameObject.h"
#include "XmlParser.h"
#include "Collider.h"

// Returns the directory portion of a file path (including the trailing slash).
static std::string DirOf(const std::string& path) {
  size_t sep = path.find_last_of("/\\");
  return (sep == std::string::npos) ? "" : path.substr(0, sep + 1);
}

TileObjects::TileObjects(const std::string &tmxFile, const std::string &tileSetFile)
    : tmxFile(tmxFile), tileSetFile(tileSetFile), tileWidth(0), tileHeight(0)
{}

void TileObjects::Load(State &state)
{
  Log::info("TILE_OBJECTS - Loading tile objects from " + tmxFile);
  LoadTmx(tmxFile);

  Log::info("TILE_OBJECTS - Spawning " + std::to_string(objects.size()) + " objects");
  for (const auto& data : objects) {
    SpawnObject(state, data);
  }
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

  GameObject *go = new GameObject();
  go->AddComponent(new TileObject(*go, data, tileSetFile, tileWidth, tileHeight));

  for (const auto& prop : data.properties) {
    const std::string& key = prop.first;
    const std::string& value = prop.second;
    
    if (value != "true") continue;

    if(key == "collider") {
      Collider *collider = new Collider(*go, Vec2(1, 1), Vec2(1, 1));
      go->AddComponent(collider);
      continue;
    }

    if (key == "pushable") {
      // TODO: Create and add pushable Component
      continue;
    }
  }

  state.AddObject(go);
}

const std::vector<TileObjectData>& TileObjects::GetObjects() const {
  return objects;
}
