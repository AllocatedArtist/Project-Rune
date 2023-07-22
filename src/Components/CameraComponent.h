#ifndef CAMERA_COMPONENT_H_
#define CAMERA_COMPONENT_H_

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

struct CameraComponent {
  float fov_ = 90.f;
  float near_ = 0.1f;
  float far_ = 100.f;

  glm::vec3 position_ = glm::vec3(0.0);
  glm::vec3 forward_ = glm::vec3(0.0, 0.0, -1.0);
  glm::vec3 right_ = glm::vec3(1.0, 0.0, 0.0);
  glm::vec3 up_ = glm::vec3(0.0, 1.0, 0.0);

  glm::vec2 aspect_ratio_;
  glm::mat4 view_ = glm::mat4(1.0);
  glm::mat4 projection_ = glm::mat4(1.0);
};

struct FlyCameraComponent {
  float sensitivity_;
  float speed_;
  float yaw_;
  float pitch_;
};

#endif