#ifndef PHYSICS_MATH_H_
#define PHYSICS_MATH_H_

#include <bullet/LinearMath/btTransform.h>
#include <glm/glm/vec3.hpp>

#include "../Components/TransformComponent.h"

btVector3 GLM_To_BT_Vec3(const glm::vec3& glm_vec);
glm::vec3 BT_To_GLM_Vec3(const btVector3& bt_vec);

btQuaternion GLM_To_BT_Quaternion(const glm::quat& glm_quat);
glm::quat BT_To_GLM_Quaternion(const btQuaternion& bt_quat);

btTransform TransformComponent_To_BT(const TransformComponent& transform);
TransformComponent BT_Transform_To_Component(const btTransform& transform, const glm::vec3& scale);


#endif