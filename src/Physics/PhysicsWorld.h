#ifndef PHYSICS_WORLD_H_
#define PHYSICS_WORLD_H_

#include <btBulletDynamicsCommon.h>
#include <glm/vec3.hpp>

#include <vector>
#include <memory>

#include "PhysicsDebugDrawer.h"

class PhysicsWorld {
public:
  PhysicsWorld();
  ~PhysicsWorld();

  void AddCollisionShape(const std::shared_ptr<btCollisionShape>& collision_shape);
  void AddMotionState(const std::shared_ptr<btMotionState>& motion_state);
  void AddRigidBody(const std::shared_ptr<btRigidBody>& body);

  void UpdateWorld();

  void EnableDebug(void);
  void DisableDebug(void);

  void SingleRayCast(const glm::vec3& from, const glm::vec3& to, btCollisionWorld::RayResultCallback& result);
private:
  std::shared_ptr<btDefaultCollisionConfiguration> physics_config_;
  std::shared_ptr<btCollisionDispatcher> physics_dispatcher_;
  std::shared_ptr<btBroadphaseInterface> physics_broadphase_;
  std::shared_ptr<btSequentialImpulseConstraintSolver> physics_solver_;
  std::shared_ptr<btDiscreteDynamicsWorld> physics_world_;

  std::vector<std::shared_ptr<btCollisionShape>> collision_shapes_;
  std::vector<std::shared_ptr<btMotionState>> motion_states_;
  std::vector<std::shared_ptr<btRigidBody>> rigid_bodies_;

  PhysicsDebugDrawer debug_drawer_;

  float time_step_ = 1.0f / 60.0f;
};


#endif