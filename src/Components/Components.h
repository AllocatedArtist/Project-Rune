#ifndef COMPONENTS_H_
#define COMPONENTS_H_

#include <entt/entt.hpp>
#include <glm/vec2.hpp>

void UpdateCameraComponents(entt::registry& registry, const glm::vec2& aspect_ratio);
void UpdateMeshComponents(entt::registry& registry);

void ReleaseMeshResources(entt::registry& registry);

#endif