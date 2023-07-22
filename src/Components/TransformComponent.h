#ifndef TRANSFORM_COMPONENT_H_
#define TRANSFORM_COMPONENT_H_

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

struct TransformComponent {
  glm::vec3 position_;
  glm::quat rotation_;
  glm::vec3 scale_;

  TransformComponent() {
    position_ = glm::vec3(0.f);
    rotation_ = glm::angleAxis(glm::radians(0.f), glm::vec3(0.f));
    scale_ = glm::vec3(1.f);
  }
};


#endif