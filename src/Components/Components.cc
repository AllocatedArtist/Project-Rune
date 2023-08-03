#include "Components.h"

#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>
#include <plog/Log.h>

#include "BoxColliderComponent.h"
#include "RigidBodyComponent.h"
#include "CameraComponent.h"
#include "MeshComponent.h"
#include "ModelComponent.h"
#include "ShaderComponent.h"
#include "TextureComponent.h"
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

void UpdateMeshComponents(entt::registry& registry) {
  auto view = registry.view<const MeshComponent, ShaderComponent>();
  auto model_view = registry.view<const ModelComponent, ShaderComponent>();

  for (auto [entity, model_component, shader_component] : model_view.each()) {
    for (const Mesh& mesh : model_component.meshes_) {
      for (const Primitive& primitive : mesh.primitives_) {
        primitive.vertex_array_->Bind();
        shader_component.shader_->Bind();

        if (primitive.texture_ != nullptr)
          primitive.texture_->BindSlot(0);
        
        glm::mat4 model_transform(1.0);

        if (registry.any_of<TransformComponent>(entity)) {
          auto transform = registry.get<TransformComponent>(entity);
          model_transform = glm::translate(model_transform, transform.position_);
          model_transform = model_transform * glm::mat4(transform.rotation_);
          model_transform = glm::scale(model_transform, transform.scale_);
        }

        glm::mat4 local_matrix(1.0);
        local_matrix = glm::translate(local_matrix, mesh.transform_.position_);
        local_matrix = local_matrix * glm::mat4(mesh.transform_.rotation_);
        local_matrix = glm::scale(local_matrix, mesh.transform_.scale_);

        shader_component.shader_->SetUniform_Float3("fragBaseColor", primitive.base_color_.x, primitive.base_color_.y, primitive.base_color_.z);
        shader_component.shader_->SetUniform_Matrix("model", model_transform * local_matrix); 
        shader_component.shader_->SetUniform_Matrix("viewProjection", Global.current_view_projection_);
 
        if (primitive.index_buffer_ > 0) {
          glDrawElements(primitive.draw_mode_, primitive.indices_count_, primitive.component_type_, 0);
        } else {
          glDrawArrays(primitive.draw_mode_, 0, 0);
        }

        if (primitive.texture_ != nullptr)
          primitive.texture_->Unbind();

        shader_component.shader_->Unbind();
        primitive.vertex_array_->Unbind();
      }
    }
  }

  for (auto entity : view) {
    auto mesh = registry.get<MeshComponent>(entity);
    auto shader = registry.get<ShaderComponent>(entity);

    mesh.vertex_array_->Bind();
    shader.shader_->Bind();

    if (registry.any_of<TextureComponent>(entity)) {
      auto texture = registry.get<TextureComponent>(entity);
      if (texture.texture_ != nullptr) {
        texture.texture_->BindSlot(0);
      }
    }

    glm::mat4 model_transform(1.0);

    if (registry.any_of<TransformComponent>(entity)) {
      auto transform = registry.get<TransformComponent>(entity);
      model_transform = glm::translate(model_transform, transform.position_);
      model_transform = model_transform * glm::mat4(transform.rotation_);
      model_transform = glm::scale(model_transform, transform.scale_);
    }

    glm::mat4 local_matrix(1.0);
    local_matrix = glm::translate(local_matrix, mesh.local_transform_.position_);
    local_matrix = local_matrix * glm::mat4(mesh.local_transform_.rotation_);
    local_matrix = glm::scale(local_matrix, mesh.local_transform_.scale_);


    shader.shader_->SetUniform_Matrix("model", model_transform * local_matrix);
 
    shader.shader_->SetUniform_Matrix("viewProjection", Global.current_view_projection_);
 
    if (mesh.index_buffer_ > 0) {
      glDrawElements(mesh.draw_mode_, mesh.num_indices_, mesh.index_type_, 0);
    }
    else {
      glDrawArrays(mesh.draw_mode_, mesh.num_vertices_, 0);
    }

    if (registry.any_of<TextureComponent>(entity)) {
      auto texture = registry.get<TextureComponent>(entity);
      if (texture.texture_ != nullptr) {
        texture.texture_->Unbind();
      }
    }

    shader.shader_->Unbind();
    mesh.vertex_array_->Unbind();
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

  auto textures = registry.view<TextureComponent>();
  for (auto [entity, texture] : textures.each()) {
    texture.texture_.reset();
  }
}
