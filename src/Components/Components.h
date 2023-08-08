#ifndef COMPONENTS_H_
#define COMPONENTS_H_

#include <entt/entt.hpp>
#include <glm/vec2.hpp>

#include "../Core/ResourceManager.h"

void UpdateCameraComponents(entt::registry& registry, const glm::vec2& aspect_ratio);
void UpdatePhysicsSystem(entt::registry& registry);
void UpdateMeshComponents(entt::registry& registry, ResourceManager& resource);

void ReleaseMeshResources(entt::registry& registry);

#endif