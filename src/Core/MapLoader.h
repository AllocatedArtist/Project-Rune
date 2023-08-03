#ifndef MAP_LOADER_H_
#define MAP_LOADER_H_

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <string>
#include <vector>

class MapLoader {
public:
  enum class CollisionType {
    kBoxType,
  };

  struct Collider {
    CollisionType type_;
    glm::vec3 position_;
    glm::quat rotation_;

    glm::vec3 size_;
  };

  MapLoader() = default;
  void LoadMap(const std::string& filename);

  const std::vector<Collider>& GetColliders() const;

private:
  std::vector<Collider> colliders_;
};


#endif