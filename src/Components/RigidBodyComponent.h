#ifndef RIGID_BODY_COMPONENT_H_
#define RIGID_BODY_COMPONENT_H_

#include <bullet/BulletDynamics/Dynamics/btRigidBody.h>
#include <bullet/LinearMath/btDefaultMotionState.h>
#include <memory>

#include "../Physics/PhysicsMath.h"
#include "../Physics/PhysicsWorld.h"

struct RigidBodyComponent {
  RigidBodyComponent() = default;
  RigidBodyComponent(PhysicsWorld& world, btCollisionShape* shape, const TransformComponent& transform, const float& mass) : mass_(mass) {
    motion_state_ = std::make_shared<btDefaultMotionState>(TransformComponent_To_BT(transform));
    btVector3 inertia(0.f, 0.f, 0.f);
    if (mass > 0.f) {
      shape->calculateLocalInertia(mass_, inertia);
    }
    btRigidBody::btRigidBodyConstructionInfo construction_info(mass_, motion_state_.get(), shape, inertia);
    rigid_body_ = std::make_shared<btRigidBody>(construction_info);

    world.AddMotionState(motion_state_);
    world.AddRigidBody(rigid_body_);
  }

  float mass_;
  std::shared_ptr<btRigidBody> rigid_body_;
  std::shared_ptr<btDefaultMotionState> motion_state_;
};

#endif