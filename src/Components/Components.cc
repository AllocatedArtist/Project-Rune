#include "Components.h"

#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>
#include <plog/Log.h>

#include "BoxColliderComponent.h"
#include "RigidBodyComponent.h"
#include "CameraComponent.h"
#include "MeshComponent.h"
#include "ModelComponent.h"
#include "TextureComponent.h"
#include "ShaderComponent.h"
#include "TransformComponent.h"
#include "InputComponent.h"

#include "../Physics/PhysicsMath.h"

#include "../Core/Time.h"
#include "../Core/Input.h"

struct {
  glm::mat4 current_view_projection_ = glm::mat4(1.0);
} Global;

void UpdateCameraComponents(entt::registry& registry, const glm::vec2& aspect_ratio) {
  auto view = registry.view<CameraComponent>();

  for (auto [entity, camera] : view.each()) {
    camera.aspect_ratio_ = aspect_ratio;
    camera.right_ = glm::normalize(glm::cross(camera.forward_, camera.up_));

    camera.view_ = glm::lookAt(camera.position_, camera.position_ + camera.forward_, camera.up_);
    camera.projection_ = glm::perspectiveFov(glm::radians(camera.fov_), camera.aspect_ratio_.x, camera.aspect_ratio_.y, camera.near_, camera.far_);   

    Global.current_view_projection_ = camera.projection_ * camera.view_;
  }

  auto fly_camera_view = registry.view<CameraComponent, FlyCameraComponent, InputComponent>();

  for (auto [entity, camera, fly_camera] : fly_camera_view.each()) {

    glm::vec3 movement_dir(0.f);

    if (Input::IsKeyDown(GLFW_KEY_W)) {
      movement_dir += camera.forward_;
    } 
    if (Input::IsKeyDown(GLFW_KEY_S)) {
      movement_dir -= camera.forward_;
    }
    if (Input::IsKeyDown(GLFW_KEY_D)) {
      movement_dir += camera.right_;
    }
    if (Input::IsKeyDown(GLFW_KEY_A)) {
      movement_dir -= camera.right_;
    }

    if (movement_dir != glm::vec3(0.f)) {
      movement_dir = glm::normalize(movement_dir);
      camera.position_ += movement_dir * fly_camera.speed_ * static_cast<float>(Time::GetDeltaTime());
    }

    if (Input::IsMouseMoving()) {
      fly_camera.yaw_ = fmodf(fly_camera.yaw_ + Input::GetMouseDelta_X() * fly_camera.sensitivity_, 360.f);
      fly_camera.pitch_ += Input::GetMouseDelta_Y() * fly_camera.sensitivity_;
      if (fly_camera.pitch_ > 89.f)
        fly_camera.pitch_ = 89.f;
      else if (fly_camera.pitch_ < -89.f)
        fly_camera.pitch_ = -89.f;
      glm::vec3 forward(0.f); 
      forward.x = glm::cos(glm::radians(fly_camera.yaw_)) * glm::cos(glm::radians(fly_camera.pitch_));
      forward.y = glm::sin(glm::radians(fly_camera.pitch_));
      forward.z = glm::sin(glm::radians(fly_camera.yaw_)) * glm::cos(glm::radians(fly_camera.pitch_));

      camera.forward_ = glm::normalize(forward);
      camera.right_ = glm::normalize(glm::cross(camera.forward_, camera.up_));
    }
  }
}

void UpdateMeshComponents(entt::registry& registry, ResourceManager& resource) {
  auto model_view = registry.view<ModelComponent, ShaderComponent>();

  for (auto [entity, model, shader] : model_view.each()) {
    for (const entt::entity& mesh_handle : model.model_resource.mesh_handles_) {
      MeshComponent mesh_component = resource.GetMeshFromHandle(mesh_handle);
      MaterialComponent material_component = resource.GetMaterialFromHandle(mesh_handle);
      TextureComponent* texture_component = resource.GetTextureFromMaterialHandle(material_component.texture_handle_);

      TransformComponent local_transform = resource.GetLocalTransformFromMeshHandle(mesh_handle);

      glm::mat4 local_transform_matrix(1.0);
      local_transform_matrix = glm::translate(local_transform_matrix, local_transform.position_);
      local_transform_matrix = local_transform_matrix * glm::mat4(local_transform.rotation_);
      local_transform_matrix = glm::scale(local_transform_matrix, local_transform.scale_);

      glm::mat4 global_transform_matrix(1.0);

      TransformComponent* global_transform = registry.try_get<TransformComponent>(entity);
      if (global_transform != nullptr)  {
        global_transform_matrix = glm::translate(global_transform_matrix, global_transform->position_);
        global_transform_matrix = global_transform_matrix * glm::mat4(global_transform->rotation_);
        global_transform_matrix = glm::scale(global_transform_matrix, global_transform->scale_); 
      }

      mesh_component.vertex_array_->Bind();
      shader.shader_->Bind();

      shader.shader_->SetUniform_Matrix("model", global_transform_matrix * local_transform_matrix);
      shader.shader_->SetUniform_Matrix("viewProjection", Global.current_view_projection_);

      shader.shader_->SetUniform_Float3("fragBaseColor", material_component.base_color_.x, material_component.base_color_.y, material_component.base_color_.z);

      if (texture_component != nullptr) {
        texture_component->texture_->BindSlot(0); 
      }
        
      glDrawElements(mesh_component.draw_mode_, mesh_component.num_indices_, mesh_component.index_type_, nullptr);

      if (texture_component != nullptr) {
        texture_component->texture_->Unbind();
      }

      shader.shader_->Unbind();
      mesh_component.vertex_array_->Unbind();
    }
  }

}

void UpdatePhysicsSystem(entt::registry& registry) {
  auto physics = registry.view<const RigidBodyComponent, TransformComponent>();

  for (auto [entity, body, transform] : physics.each()) {
    glm::vec3 scale = transform.scale_;
    btTransform new_transform;
    body.motion_state_->getWorldTransform(new_transform);

    transform = BT_Transform_To_Component(new_transform, scale);
  }
}

void ReleaseMeshResources(entt::registry& registry) {
  auto meshes = registry.view<MeshComponent>();
  for (auto [entity, mesh] : meshes.each()) {
    mesh.vertex_array_.reset();
    mesh.vertex_buffer_.reset();
    mesh.index_buffer_.reset();
  }

  auto shaders = registry.view<ShaderComponent>();
  for (auto [entity, shader] : shaders.each()) {
    shader.shader_.reset();
  }
}
