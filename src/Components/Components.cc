#include "Components.h"

#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>
#include <plog/Log.h>

#include "CameraComponent.h"
#include "MeshComponent.h"
#include "ShaderComponent.h"
#include "TextureComponent.h"
#include "TransformComponent.h"
#include "InputComponent.h"

#include "../Core/Application.h"
#include "../Core/Input.h"

struct {
  glm::mat4 current_view_projection_ = glm::mat4(1.0);
} Global;

void UpdateCameraComponents(entt::registry& registry, const glm::vec2& aspect_ratio) {
  auto view = registry.view<CameraComponent>();

  for (auto entity : view) {
    auto camera = registry.get<CameraComponent>(entity);
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
      camera.position_ += movement_dir * fly_camera.speed_ * Application::GetDeltaTime();
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

  for (auto entity : view) {
    auto mesh = registry.get<MeshComponent>(entity);
    auto shader = registry.get<ShaderComponent>(entity);

    mesh.vertex_array_->Bind();
    shader.shader_->Bind();

    if (registry.any_of<TextureComponent>(entity)) {
      auto texture = registry.get<TextureComponent>(entity);
      texture.texture_->BindSlot(0);
    }

    glm::mat4 model_transform(1.0);

    if (registry.any_of<TransformComponent>(entity)) {
      auto transform = registry.get<TransformComponent>(entity);
      model_transform = glm::translate(model_transform, transform.position_);
      model_transform = model_transform * glm::mat4(transform.rotation_);
      model_transform = glm::scale(model_transform, transform.scale_);
    }

    shader.shader_->SetUniform_Matrix("model", model_transform);
 
    shader.shader_->SetUniform_Matrix("viewProjection", Global.current_view_projection_);
 
    if (mesh.index_buffer_ > 0) {
      glDrawElements(GL_TRIANGLES, mesh.num_indices_, GL_UNSIGNED_SHORT, 0);
    }
    else {
      glDrawArrays(GL_TRIANGLES, mesh.num_vertices_, 0);
    }

    shader.shader_->Unbind();
    mesh.vertex_array_->Unbind();
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
