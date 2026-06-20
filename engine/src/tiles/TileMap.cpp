#include "GameObject.h"
#include "TileMap.h"
#include "Log.h"
#include "Camera.h"
#include "XmlParser.h"
#include <fstream>
#include <sstream>
#include <algorithm>

TileMap::TileMap(GameObject &associated, std::string file, TileSet *tileSet)
: Component(associated), scale(Vec2(1.0f, 1.0f))
{
  if (tileSet) {
    // Default to firstgid 1 if a single TileSet is provided via constructor
    tileSets.push_back({1, std::unique_ptr<TileSet>(tileSet)});
  }
  Load(file);
}

void TileMap::Load(std::string file) {
  Log::info("TileMap - Loading tile map from file: " + file);
  std::string ext = file.size() >= 4 ? file.substr(file.size() - 4) : "";
  isTmx = (ext == ".tmx");

  if (isTmx) {
    LoadTmx(file);
  } else {
    LoadTxt(file);
  }
}

void TileMap::LoadTxt(const std::string& file) {
  std::ifstream in(file);

  if (!in.is_open()) {
    throw std::runtime_error("Failed to open tile map file: " + file);
  }

  std::string line;

  if (!std::getline(in, line)) {
    throw std::runtime_error("TileMap::Load: Failed to read map dimensions.");
  }

  std::stringstream ss(line);
  std::string value;

  std::getline(ss, value, ',');
  mapWidth = std::stoi(value);

  std::getline(ss, value, ',');
  mapHeight = std::stoi(value);

  std::getline(ss, value, ',');
  mapDepth = std::stoi(value);

  tileMatrix.clear();
  int tilesToRead = mapWidth * mapHeight * mapDepth;
  int tilesRead = 0;

  while (tilesRead < tilesToRead && std::getline(in, line)) {
    if (line.find_first_not_of(", \t\r\n") == std::string::npos) {
      // Jumps the empty lines between the dimensions and the tile data
      continue;
    }

    std::stringstream linestream(line);
    int tilesInRow = 0;

    while (tilesInRow < mapWidth && std::getline(linestream, value, ','))
    {
      if (value.empty()) {
        Log::warning("TileMap::Load: Empty tile value found, defaulting to -1.");
        tileMatrix.push_back(-1);
      }

      try {
		// evitar problema de overflow stoi -> stoll
		int tile = (int)(std::stoll(value) & 0x0FFFFFFF);
        tileMatrix.push_back(tile);

        ++tilesRead;
        ++tilesInRow;
      } catch (const std::exception& e) {
        throw std::runtime_error("TileMap::Load: Invalid tile index '" + value + "'.");
      }
    }
  }

  while (tilesRead < tilesToRead) {
    throw std::runtime_error("TileMap::Load: Not enough tile data in file.");
  }

  in.close();
}

void TileMap::LoadTmx(const std::string& file) {
  XmlNode root = XmlParser::ParseFile(file);

  if (root.tag != "map") {
    throw std::runtime_error("TileMap::LoadTmx: Root element is not <map> in " + file);
  }

  // --- Parse Tilesets ---
  tileSets.clear();
  std::vector<const XmlNode*> tilesetNodes = root.findChildren("tileset");
  for (const auto* tsNode : tilesetNodes) {
    int firstgid = std::stoi(tsNode->getAttribute("firstgid", "1"));
    std::string source = tsNode->getAttribute("source");
    
    if (source.empty()) {
      throw std::runtime_error("TileMap::LoadTmx: Tileset source is empty in " + file);
    }

    // Resolve source path relative to map file
    std::string mapDir = file.substr(0, file.find_last_of("/\\") + 1);
    std::string tsxPath = mapDir + source;

    XmlNode tsx = XmlParser::ParseFile(tsxPath);
    int tw = std::stoi(tsx.getAttribute("tilewidth", "0"));
    int th = std::stoi(tsx.getAttribute("tileheight", "0"));

    // Extract the actual image path from the <image> tag inside the .tsx file
    const XmlNode* imageNode = tsx.findChild("image");
    std::string imagePath = "";
    if (imageNode) {
      imagePath = imageNode->getAttribute("source");
      
      // Resolve image path relative to the .tsx file directory
      std::string tsxDir = tsxPath.substr(0, tsxPath.find_last_of("/\\") + 1);
      imagePath = tsxDir + imagePath;
    } else {
      throw std::runtime_error("TileMap::LoadTmx: <image> tag not found in tileset file: " + tsxPath);
    }

    tileSets.push_back(std::make_pair(firstgid, std::unique_ptr<TileSet>(new TileSet(tw, th, imagePath))));
  }

  // Sort tilesets by firstgid to allow efficient lookup
  std::sort(tileSets.begin(), tileSets.end(), [](const std::pair<int, std::unique_ptr<TileSet>>& a, const std::pair<int, std::unique_ptr<TileSet>>& b) {
    return a.first < b.first;
  });

  mapWidth  = std::stoi(root.getAttribute("width",  "0"));
  mapHeight = std::stoi(root.getAttribute("height", "0"));
  
  std::vector<const XmlNode*> layers = root.findChildren("layer");
  mapDepth = static_cast<int>(layers.size());

  tileMatrix.clear();

  for (const auto* layerNode : layers) {
    const XmlNode* data = layerNode->findChild("data");
    if (!data) {
      throw std::runtime_error("TileMap::LoadTmx: No <data> element found in layer in " + file);
    }

    std::string encoding = data->getAttribute("encoding");
    if (encoding != "csv") {
      throw std::runtime_error("TileMap::LoadTmx: Only CSV encoding is supported, got: " + encoding);
    }

    std::stringstream ss(data->text);
    std::string token;
    while (std::getline(ss, token, ',')) {
      size_t start = token.find_first_not_of(" \t\r\n");
      if (start == std::string::npos) continue; 
      size_t end = token.find_last_not_of(" \t\r\n");
      token = token.substr(start, end - start + 1);
      if (token.empty()) continue;
	  // stoll para nao dar erro, e convertemos (int) para ignorar os bits gigantes
	  // O problema era quando invertiamos uma sprite no tiled dava overflow
	  tileMatrix.push_back((int)std::stoll(token));
    }
  }

  int expected = mapWidth * mapHeight * mapDepth;
  if (static_cast<int>(tileMatrix.size()) != expected) {
    throw std::runtime_error(
      "TileMap::LoadTmx: Expected " + std::to_string(expected) +
      " tiles, got " + std::to_string(tileMatrix.size()));
  }
}

void TileMap::SetTileSet(TileSet *tileSet) {
  tileSets.clear();
  if (tileSet) {
    tileSets.push_back({1, std::unique_ptr<TileSet>(tileSet)});
  }
}

int& TileMap::At(int x, int y, int z)
{
  if (x < 0 ||
      y < 0 ||
      z < 0 ||
      x >= mapWidth ||
      y >= mapHeight ||
      z >= mapDepth) {

    throw std::out_of_range("TileMap::At: Index out of range.");
  }

  int col = x;
  int row = y * mapWidth;
  int layer = z * mapWidth * mapHeight;

  return tileMatrix[col + row + layer];
}



void TileMap::RenderLayer() {
  if (isTmx) {
    RenderLayerTmx();
  } else {
    RenderLayerTxt();
  }
}

void TileMap::RenderLayerTxt() {
  if (tileSets.empty()) {
    Log::error("TileMap::RenderLayerTxt: No TileSets associated with TileMap.");
    return;
  }

  float parallaxFactor = 0.0;
  float parallaxIncrement = 1.0f / mapDepth;

  for (int z = 0; z < mapDepth; ++z) {
    parallaxFactor += parallaxIncrement;

    Camera::GetInstance().SetSpeedMultiplier(parallaxFactor);

    for (int y = 0; y < mapHeight; ++y)
    {
      for (int x = 0; x < mapWidth; ++x)
      {
        int tileIndex = At(x, y, z);

        // Since RenderLayerTxt is likely for legacy simple maps, 
        // we use the first tileset if available.
        if (!tileSets.empty()) {
            tileSets[0].second->RenderTile(tileIndex,
                                        associated.box.x + x * tileSets[0].second->GetTileWidth(),
                                        associated.box.y + y * tileSets[0].second->GetTileHeight());
        }
      }
    }

    Camera::GetInstance().ResetSpeedMultiplier();
  }
}

void TileMap::RenderLayerTmx() {
  if (tileSets.empty()) {
    Log::error("TileMap::RenderLayerTmx: No TileSets associated with TileMap.");
    return;
  }

  // TMX tile IDs are 1-based; 0 means empty (skip rendering).
  float parallaxFactor = 0.0;
  float parallaxIncrement = 1.0f / mapDepth;

  for (int z = 0; z < mapDepth; ++z) {
    parallaxFactor += parallaxIncrement;

    // Camera::GetInstance().SetSpeedMultiplier(parallaxFactor);

    for (int y = 0; y < mapHeight; ++y)
    {
      for (int x = 0; x < mapWidth; ++x)
      {
        int rawId = At(x, y, z);

        if (rawId == 0) {
          continue; // empty tile
        }

        // Find the correct tileset for the rawId
        TileSet* targetTileSet = nullptr;
        int localIndex = 0;

        for (auto it = tileSets.rbegin(); it != tileSets.rend(); ++it) {
          if (rawId >= it->first) {
            targetTileSet = it->second.get();
            localIndex = rawId - it->first;
            break;
          }
        }

        if (!targetTileSet) {
          Log::warning("TileMap::RenderLayerTmx: Tile ID " + std::to_string(rawId) + " does not belong to any loaded tileset.");
          continue;
        }

        targetTileSet->RenderTile(localIndex,
                                associated.box.x + x * targetTileSet->GetTileWidth() * scale.x,
                                associated.box.y + y * targetTileSet->GetTileHeight() * scale.y,
                                scale);
      }
    }

    Camera::GetInstance().ResetSpeedMultiplier();
  }
}

void TileMap::Render() {
  RenderLayer();
}

void TileMap::Update(float dt) {}

int TileMap::GetWidth() {
  return this->mapWidth;
}

int TileMap::GetHeight() {
  return this->mapHeight;
}

int TileMap::GetDepth() {
  return this -> mapDepth;
}

std::vector<int> TileMap::GetTileMatrix()
{
  return this->tileMatrix;
}
