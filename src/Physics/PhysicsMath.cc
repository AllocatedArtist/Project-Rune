#include "PhysicsMath.h"

btVector3 GLM_To_BT_Vec3(const glm::vec3& glm_vec) {
  return btVector3(glm_vec.x, glm_vec.y, glm_vec.z);
}

glm::vec3 BT_To_GLM_Vec3(const btVector3& bt_vec) {
  return glm::vec3(bt_vec.getX(), bt_vec.getY(), bt_vec.getZ());
}

btQuaternion GLM_To_BT_Quaternion(const glm::quat& glm_quat) {
  return btQuaternion(glm_quat.x, glm_quat.y, glm_quat.z, glm_quat.w);
}

glm::quat BT_To_GLM_Quaternion(const btQuaternion& bt_quat) {
  return glm::quat(bt_quat.getW(), bt_quat.getX(), bt_quat.getY(), bt_quat.getZ());
}

btTransform TransformComponent_To_BT(const TransformComponent& transform) {
  btTransform bt_transform;
  bt_transform.setIdentity();
  bt_transform.setOrigin(GLM_To_BT_Vec3(transform.position_));
  bt_transform.setRotation(GLM_To_BT_Quaternion(transform.rotation_));
  return bt_transform;
}

TransformComponent BT_Transform_To_Component(const btTransform& transform, const glm::vec3& scale) {
  TransformComponent transform_component;
  transform_component.position_ = BT_To_GLM_Vec3(transform.getOrigin());
  transform_component.rotation_ = BT_To_GLM_Quaternion(transform.getRotation());
  transform_component.scale_ = scale;
  return transform_component;
}

