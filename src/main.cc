#include <glad/glad.h>
#include <plog/Log.h>

#include <iostream>

#include "Core/Application.h"
#include "Core/ResourceManager.h"
#include "Core/Time.h"
#include "Core/Input.h"

#include "Core/MapLoader.h"

#include "Graphics/VertexArray.h"
#include "Graphics/Buffer.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/ModelLoader.h"

#include "Graphics/DebugDrawer.h"

#include "Components/RigidBodyComponent.h"
#include "Components/BoxColliderComponent.h"
#include "Components/SphereColliderComponent.h"
#include "Components/ModelComponent.h"
#include "Components/MeshComponent.h"
#include "Components/ShaderComponent.h"
#include "Components/InputComponent.h"
#include "Components/CameraComponent.h"
#include "Components/TransformComponent.h"
#include "Components/Components.h"

#include "Gui/ImGui_Backend.h"

#include "Physics/PhysicsWorld.h"
#include "Physics/PhysicsMath.h"

struct {
  Application app_ = Application(1600, 1480, "Project Rune");

  ResourceManager resource_manager_;

  entt::registry registry_;
  entt::entity camera_;

  PhysicsWorld physics_world;
} Core;

struct {
  bool draw_debug_ = false;
} UI;

void DrawUI(void) {
  if (ImGui::Begin("My Window", nullptr, ImGuiWindowFlags_NoResize)) {
    ImGui::SetWindowSize(ImVec2(300.f, 300.f));
    ImGui::SetWindowFontScale(2.4f);
    ImGui::Text("Frames: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
    ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);

    if (ImGui::Checkbox("Debug Draw", &UI.draw_debug_)) {
      if (UI.draw_debug_) {
        Core.physics_world.EnableDebug();
      } else {
        Core.physics_world.DisableDebug();
      }
    }
  }
  ImGui::End();
}

void ClearBackgroundColor(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0.95, 0.75, 0.75, 1.0); 

  if (Input::IsKeyPressed(GLFW_KEY_ESCAPE)) {
    Core.app_.Quit();
  }

  if (Input::IsKeyPressed(GLFW_KEY_Q)) {
    if (Input::GetCursorState() == Input::CursorState::kCursorStateDisabled) {
      Core.registry_.remove<InputComponent>(Core.camera_);
      Input::SetCursorState(Input::CursorState::kCursorStateNormal);
    }
    else {
      Core.registry_.emplace_or_replace<InputComponent>(Core.camera_);
      Input::SetCursorState(Input::CursorState::kCursorStateDisabled);
    }
  }  
}

void Setup_PhysicsDemo() {  

  Core.resource_manager_.LoadModelAsset("../../assets/map3.gltf");
  Core.resource_manager_.LoadShaderAsset("../../assets/shader.glsl");

  ShaderResource shader_resource = Core.resource_manager_.GetShaderResource("../../assets/shader.glsl");
  ShaderComponent& shader_component = Core.resource_manager_.GetShaderFromHandle(shader_resource.shader_handle_);

  shader_component.shader_
    ->LoadUniform("model")
    .LoadUniform("viewProjection")
    .LoadUniform("fragBaseColor")
    .LoadUniform("texture0")
    .SetUniform_Int("texture0", 0);

  entt::entity minecraft_pp = Core.registry_.create();

  TransformComponent transform{};
  transform.position_ = glm::vec3(0.f, 0.f, 0.f);

  Core.registry_.emplace<ModelComponent>(minecraft_pp, Core.resource_manager_.GetModelResource("../../assets/map3.gltf"));
  Core.registry_.emplace<TransformComponent>(minecraft_pp, transform);
  Core.registry_.emplace<ShaderComponent>(minecraft_pp, shader_component);

  Core.registry_.emplace<InputComponent>(Core.camera_);
  Core.registry_.emplace<FlyCameraComponent>(Core.camera_, FlyCameraComponent(0.1f, 10.f));
  Core.registry_.emplace<CameraComponent>(Core.camera_, CameraComponent(glm::vec3(0.f, 2.f, 10.f), 90.f, 0.01f, 100.f));

  Input::SetCursorState(Input::CursorState::kCursorStateDisabled);
}

void Update(void) {
  Core.physics_world.UpdateWorld();
  UpdatePhysicsSystem(Core.registry_); 
}

void DrawDebug(void) {
  CameraComponent camera = Core.registry_.get<CameraComponent>(Core.camera_);
  DebugDrawer::DrawSquares(camera.projection_ * camera.view_);
  DebugDrawer::DrawLines(camera.projection_ * camera.view_);
}

int main(void) {
  Application::SetTargetFPS(120);

  DebugDrawer::InitializeDebugDrawer(); 

  Core.app_
    .AddSystem(Application::SystemType::kSystemStart, ImGui_Backend::Start)
    .AddSystem(Application::SystemType::kSystemStart, Setup_PhysicsDemo)
    .AddSystem(Application::SystemType::kSystemUpdate, ImGui_Backend::NewFrame)
    .AddSystem(Application::SystemType::kSystemUpdate, Update)
    .AddSystem(Application::SystemType::kSystemUpdate, DrawUI)
    .AddSystem(Application::SystemType::kSystemUpdate, ClearBackgroundColor)
    .AddSystem(Application::SystemType::kSystemUpdate, [](){ 
      UpdateCameraComponents(Core.registry_, glm::vec2(Core.app_.GetWindowWidth(), Core.app_.GetWindowHeight())); 
    })
    .AddSystem(Application::SystemType::kSystemUpdate, [](){ UpdateMeshComponents(Core.registry_, Core.resource_manager_); })
    .AddSystem(Application::SystemType::kSystemUpdate, DrawDebug)
    .AddSystem(Application::SystemType::kSystemUpdate, ImGui_Backend::Render)
    .AddSystem(Application::SystemType::kSystemEnd, [](){ ReleaseMeshResources(Core.registry_); })
    .AddSystem(Application::SystemType::kSystemEnd, ImGui_Backend::End)
    .Run();
  

  std::cout << "Project Rune!" << std::endl;
  return 0;
}