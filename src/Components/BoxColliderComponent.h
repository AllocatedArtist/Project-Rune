#ifndef BOX_COLLIDER_COMPONENT_H_
#define BOX_COLLIDER_COMPONENT_H_

#include <bullet/BulletCollision/CollisionShapes/btBoxShape.h>
#include <glm/vec3.hpp>
#include <memory>

#include "../Physics/PhysicsWorld.h"
#include "../Physics/PhysicsMath.h"

struct BoxColliderComponent {
  BoxColliderComponent() = default;
  BoxColliderComponent(PhysicsWorld& world, const glm::vec3& scale) : scale_(scale) {
    box_shape_ = std::make_shared<btBoxShape>(GLM_To_BT_Vec3(scale_));
    world.AddCollisionShape(box_shape_);
  }

  glm::vec3 scale_;
  std::shared_ptr<btBoxShape> box_shape_;
};

#endif