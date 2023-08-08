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

  CameraComponent(const glm::vec3& position, const float& fov, const float& near_clip, const float& far_clip) { 
    forward_ = glm::vec3(0.f, 0.f, -1.f);
    up_ = glm::vec3(0.f, 1.f, 0.f);
    position_ = position;

    fov_ = fov;
    near_ = near_clip;
    far_ = far_clip;
  }
};


struct FlyCameraComponent {
  float sensitivity_ = 0.f;
  float speed_ = 0.f;
  float yaw_ = 0.f;
  float pitch_ = 0.f;

  FlyCameraComponent(const float& sensitivity, const float& speed) : sensitivity_(sensitivity), speed_(speed) {
    yaw_ = 0.f;
    pitch_ = 0.f;
  }
};


#endif