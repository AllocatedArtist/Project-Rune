#include "PhysicsWorld.h"

#include <plog/Log.h>

#include "PhysicsMath.h"

PhysicsWorld::PhysicsWorld() {
  physics_config_ = std::make_shared<btDefaultCollisionConfiguration>();
  physics_dispatcher_ = std::make_shared<btCollisionDispatcher>(physics_config_.get());
  physics_broadphase_ = std::make_shared<btDbvtBroadphase>();
  physics_solver_ = std::make_shared<btSequentialImpulseConstraintSolver>();
  physics_world_ = std::make_shared<btDiscreteDynamicsWorld>(physics_dispatcher_.get(), physics_broadphase_.get(), physics_solver_.get(), physics_config_.get());

  physics_world_->setGravity(btVector3(0.f, -9.81f, 0.f));
  physics_world_->setDebugDrawer(&debug_drawer_);

  PLOGD << "Physics World created";
}

PhysicsWorld::~PhysicsWorld() {
  PLOGD << "Physics world destroyed";
}

void PhysicsWorld::AddCollisionShape(const std::shared_ptr<btCollisionShape>& collision_shape) {
  collision_shapes_.push_back(collision_shape);
}

void PhysicsWorld::AddMotionState(const std::shared_ptr<btMotionState>& motion_state) {
  motion_states_.push_back(motion_state);
}

void PhysicsWorld::AddRigidBody(const std::shared_ptr<btRigidBody>& body) {
  rigid_bodies_.push_back(body);
  physics_world_->addRigidBody(body.get());
}

void PhysicsWorld::UpdateWorld() {
  physics_world_->stepSimulation(time_step_, 10);
  physics_world_->debugDrawWorld();
}

void PhysicsWorld::EnableDebug(void) {
  physics_world_->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawContactPoints);
}

void PhysicsWorld::DisableDebug(void) {
  physics_world_->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_NoDebug);
}


void PhysicsWorld::SingleRayCast(const glm::vec3& from, const glm::vec3& to, btCollisionWorld::RayResultCallback& result) {
  physics_world_->rayTest(GLM_To_BT_Vec3(from), GLM_To_BT_Vec3(to), result);
}
