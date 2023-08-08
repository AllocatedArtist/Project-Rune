#ifndef MATERIAL_COMPONENT_H_
#define MATERIAL_COMPONENT_H_

#include <entt/entt.hpp>
#include <glm/vec3.hpp>

struct MaterialComponent {
  entt::entity texture_handle_;
  glm::vec3 base_color_;
};


#endif