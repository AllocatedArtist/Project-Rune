#ifndef CAMERA_COMPONENT_H_
#define CAMERA_COMPONENT_H_

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

struct CameraComponent {
  float fov_;
  float near_;
  float far_;

  glm::vec3 position_;
  glm::vec3 forward_;
  glm::vec3 right_;
  glm::vec3 up_;

  glm::vec2 aspect_ratio_;
  glm::mat4 view_;
  glm::mat4 projection_;
};

#endif