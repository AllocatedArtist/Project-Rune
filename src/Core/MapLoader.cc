#include "MapLoader.h"

#include <tinygltf/json.hpp>
#include <plog/Log.h>

#include <filesystem>
#include <fstream>


static void LoadCollisions(std::vector<MapLoader::Collider>& colliders, const nlohmann::json& collisions) {
  for (const auto& object : collisions) {
    std::vector<float> pos;
    std::vector<float> size;
    std::vector<float> rotation;
    std::string type;

    object.at("Position").get_to(pos);
    object.at("Rotation").get_to(rotation);
    object.at("Size").get_to(size);
    object.at("Type").get_to(type);

    if (type == "Box") {
      colliders.push_back(
        MapLoader::Collider { 
          MapLoader::CollisionType::kBoxType, 
          glm::vec3(pos[0], pos[1], pos[2]),
          glm::quat(rotation[0], rotation[1], rotation[2], rotation[3]),
          glm::vec3(size[0] * 0.5f, size[1] * 0.5f, size[2] * 0.5f) //Scale down to half extents
      });
    }
  }
}

void MapLoader::LoadMap(const std::string& filename) {

  assert(std::filesystem::exists(filename) && "File does not exist");
  PLOG_ERROR_IF(!std::filesystem::exists(filename))  << filename << " does not exist";

  std::ifstream mapdata(filename);
  nlohmann::json data = nlohmann::json::parse(mapdata);

  auto collisions = data["COLLISION"];
  if (!collisions.empty()) { 
    LoadCollisions(colliders_, collisions);
    PLOGD << "Collision layer loaded";
  }

  PLOGD << "Map loaded successfully from: " << filename;
}

const std::vector<MapLoader::Collider>& MapLoader::GetColliders() const {
  return colliders_;
}
