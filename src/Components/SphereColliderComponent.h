#ifndef SPHERE_COLLIDER_COMPONENT_H_
#define SPHERE_COLLIDER_COMPONENT_H_

#include <bullet/BulletCollision/CollisionShapes/btSphereShape.h>
#include <glm/vec3.hpp>
#include <memory>

#include "../Physics/PhysicsWorld.h"
#include "../Physics/PhysicsMath.h"

struct SphereColliderComponent {
  SphereColliderComponent() = default;
  SphereColliderComponent(PhysicsWorld& world, const float& radius) : radius_(radius) {
    sphere_shape_ = std::make_shared<btSphereShape>(radius_);
    world.AddCollisionShape(sphere_shape_);
  }

  float radius_;
  std::shared_ptr<btSphereShape> sphere_shape_;
};


#endif