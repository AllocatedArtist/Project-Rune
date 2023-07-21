#include "Components.h"

#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>
#include <plog/Log.h>

#include "CameraComponent.h"
#include "MeshComponent.h"
#include "ShaderComponent.h"
#include "TextureComponent.h"

void UpdateCameraComponents(entt::registry& registry, const glm::vec2& aspect_ratio) {
  auto view = registry.view<CameraComponent>();

  for (auto entity : view) {
    auto camera = registry.get<CameraComponent>(entity);
    camera.aspect_ratio_ = aspect_ratio;
    camera.view_ = glm::lookAt(camera.position_, camera.position_ + camera.forward_, camera.up_);
    camera.projection_ = glm::perspectiveFov(glm::radians(camera.fov_), camera.aspect_ratio_.x, camera.aspect_ratio_.y, camera.near_, camera.far_);   
    camera.right_ = glm::normalize(glm::cross(camera.forward_, camera.up_));
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
