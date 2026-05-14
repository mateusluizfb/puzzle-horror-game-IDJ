#include "GameObject.h"
#include "TileMap.h"
#include "Log.h"
#include "Camera.h"
#include "XmlParser.h"
#include <fstream>
#include <sstream>

TileMap::TileMap(GameObject &associated, std::string file, TileSet *tileSet)
: Component(associated), tileSet(tileSet), scale(1.0f)
{
  Load(file);
}

void TileMap::Load(std::string file) {
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
        int tile = std::stoi(value);
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

  mapWidth  = std::stoi(root.getAttribute("width",  "0"));
  mapHeight = std::stoi(root.getAttribute("height", "0"));
  mapDepth  = 1; // single-layer; extend via multiple <layer> nodes later

  tileMatrix.clear();

  const XmlNode* layer = root.findChild("layer");
  if (!layer) {
    throw std::runtime_error("TileMap::LoadTmx: No <layer> found in " + file);
  }

  const XmlNode* data = layer->findChild("data");
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

    if (start == std::string::npos) continue; // empty / whitespace-only token

    size_t end = token.find_last_not_of(" \t\r\n");

    token = token.substr(start, end - start + 1);

    if (token.empty()) continue;
    
    tileMatrix.push_back(std::stoi(token));
  }

  int expected = mapWidth * mapHeight * mapDepth;
  if (static_cast<int>(tileMatrix.size()) != expected) {
    throw std::runtime_error(
      "TileMap::LoadTmx: Expected " + std::to_string(expected) +
      " tiles, got " + std::to_string(tileMatrix.size()));
  }
}

void TileMap::SetTileSet(TileSet *tileSet) {
  this->tileSet.reset(tileSet);
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
  if (!tileSet) {
    Log::error("TileMap::RenderLayerTxt: No TileSet associated with TileMap.");
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

        tileSet->RenderTile(tileIndex,
                            associated.box.x + x * tileSet->GetTileWidth(),
                            associated.box.y + y * tileSet->GetTileHeight());
      }
    }

    Camera::GetInstance().ResetSpeedMultiplier();
  }
}

void TileMap::RenderLayerTmx() {
  if (!tileSet) {
    Log::error("TileMap::RenderLayerTmx: No TileSet associated with TileMap.");
    return;
  }

  // TMX tile IDs are 1-based; 0 means empty (skip rendering).
  float parallaxFactor = 0.0;
  float parallaxIncrement = 1.0f / mapDepth;

  for (int z = 0; z < mapDepth; ++z) {
    parallaxFactor += parallaxIncrement;

    Camera::GetInstance().SetSpeedMultiplier(parallaxFactor);

    for (int y = 0; y < mapHeight; ++y)
    {
      for (int x = 0; x < mapWidth; ++x)
      {
        int rawId = At(x, y, z);

        if (rawId == 0) {
          continue; // empty tile
        }

        unsigned tileIndex = static_cast<unsigned>(rawId - 1);

        tileSet->RenderTile(tileIndex,
                            associated.box.x + x * tileSet->GetTileWidth(),
                            associated.box.y + y * tileSet->GetTileHeight(),
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
