#include "PhysicsDebugDrawer.h"
#include <plog/Log.h>

#include "../Graphics/DebugDrawer.h"
#include "PhysicsMath.h"

void PhysicsDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
  DebugDrawer::CreateLine(BT_To_GLM_Vec3(from), BT_To_GLM_Vec3(to), BT_To_GLM_Vec3(color));
}

void PhysicsDebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) {
  DebugDrawer::CreateSquare(BT_To_GLM_Vec3(PointOnB), BT_To_GLM_Vec3(color), lifeTime);
  DebugDrawer::CreateLine(BT_To_GLM_Vec3(PointOnB), BT_To_GLM_Vec3(normalOnB), BT_To_GLM_Vec3(color));
}

void PhysicsDebugDrawer::reportErrorWarning(const char* warningString) {
  PLOG_WARNING << "PHYSICS DEBUG: " << warningString;
}

void PhysicsDebugDrawer::draw3dText(const btVector3& location, const char* textString) {
  PLOG_INFO << "PHYSICS DEBUG: " << textString;
}

void PhysicsDebugDrawer::setDebugMode(int debugMode) {
  debug_mode_ = debugMode;
}

int PhysicsDebugDrawer::getDebugMode() const {
  return debug_mode_;
}

void PhysicsDebugDrawer::clearLines() {
  DebugDrawer::FlushLines();
}



