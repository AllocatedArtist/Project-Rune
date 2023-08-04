#include <glad/glad.h>
#include <plog/Log.h>

#include <iostream>

#include "Core/Application.h"
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
#include "Components/TextureComponent.h"
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
  entt::registry registry_;
  entt::entity camera_;
  std::vector<entt::entity> model_entity;
  PhysicsWorld physics_world;

  ModelComponent ball_model_;
  SphereColliderComponent sphere_component_;

  std::shared_ptr<Shader> default_shader_;

  MapLoader level_;
} Core;

void Setup_Buffers() {
  std::shared_ptr<Shader> shader = std::make_shared<Shader>("../../assets/shader.glsl");
  shader->Bind();
  shader->LoadUniform("texture0").SetUniform_Int("texture0", 0);
  shader->LoadUniform("model").LoadUniform("viewProjection");
  shader->Unbind();

  TransformComponent better_t;
  better_t.position_ = glm::vec3(0.f, 3.f, -5.f);
  better_t.scale_ = glm::vec3(0.6f);

  TransformComponent grass_t;
  grass_t.position_ = glm::vec3(0.f, 1.f, -0.5f);
  grass_t.scale_ = glm::vec3(0.3f, 0.3f, 0.3f);

  TransformComponent map_t;
  map_t.position_ = glm::vec3(0.f, 0.f, -3.5f);

  entt::entity e = Core.registry_.create();
  Core.registry_.emplace<ModelComponent>(e, "../../assets/cube2.gltf");
  Core.registry_.emplace<TransformComponent>(e, better_t);
  Core.registry_.emplace<ShaderComponent>(e, shader);

  Core.model_entity.push_back(e);

  entt::entity e2 = Core.registry_.create();
  Core.registry_.emplace<ModelComponent>(e2, "../../assets/grassblock.gltf");
  Core.registry_.emplace<TransformComponent>(e2, grass_t);
  Core.registry_.emplace<ShaderComponent>(e2, shader);

  Core.model_entity.push_back(e2);

  entt::entity map = Core.registry_.create();
  Core.registry_.emplace<ModelComponent>(map, "../../assets/map.gltf");
  Core.registry_.emplace<TransformComponent>(map, map_t);
  Core.registry_.emplace<ShaderComponent>(map, shader);

  Core.camera_ = Core.registry_.create();

  CameraComponent camera_component{};
  camera_component.near_ = 0.01f;
  camera_component.far_ = 100.f;
  camera_component.fov_ = 90.f;
  camera_component.position_ = glm::vec3(0.f, 2.f, 1.f);
  camera_component.forward_ = glm::vec3(0.f, 0.f, -1.f);
  camera_component.up_ = glm::vec3(0.f, 1.f, 0.f);

  Core.registry_.emplace<InputComponent>(Core.camera_);
  Core.registry_.emplace<FlyCameraComponent>(Core.camera_, 0.2f, 1.0f);
  Core.registry_.emplace<CameraComponent>(Core.camera_, camera_component);

  Input::SetCursorState(Input::CursorState::kCursorStateDisabled);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
}

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
        DebugDrawer::FlushSquares();
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

  for (int i = 0; i < Core.model_entity.size(); ++i) {
    auto& t = Core.registry_.get<TransformComponent>(Core.model_entity[i]);
    float delta = static_cast<float>(Time::GetDeltaTime());
    t.rotation_ = glm::rotate(t.rotation_, (i % 2 == 0) ? delta : -delta, glm::vec3(1.f, 1.f, 0.f));
  } 
}

void Setup_PhysicsDemo() {

  std::shared_ptr<Shader> shader = std::make_shared<Shader>("../../assets/shader.glsl");
  shader->LoadUniform("texture0").SetUniform_Int("texture0", 0);
  shader->LoadUniform("model").LoadUniform("viewProjection");
  shader->LoadUniform("fragBaseColor");

  Core.default_shader_ = shader;

  entt::entity floor = Core.registry_.create();  

  ModelComponent floor_model("../../assets/map3.gltf");
  Core.level_.LoadMap("../../assets/leveldata/level3.json");

  TransformComponent floor_transform;
  floor_transform.position_ = glm::vec3(0.f, 0.f, 0.f);
  floor_transform.scale_ = glm::vec3(1.0f, 1.0f, 1.0f);

  Core.registry_.emplace<ModelComponent>(floor, floor_model);
  Core.registry_.emplace<ShaderComponent>(floor, Core.default_shader_);
  Core.registry_.emplace<TransformComponent>(floor, floor_transform);

  for (const MapLoader::Collider& collider : Core.level_.GetColliders()) {
    TransformComponent static_transform;
    static_transform.position_ = collider.position_;
    static_transform.rotation_ = collider.rotation_;

    if (collider.type_ == MapLoader::CollisionType::kBoxType) {
      BoxColliderComponent static_collider(Core.physics_world, collider.size_);
      entt::entity static_body = Core.registry_.create();
      Core.registry_.emplace<BoxColliderComponent>(static_body, static_collider);
      Core.registry_.emplace<TransformComponent>(static_body, static_transform);
      Core.registry_.emplace<RigidBodyComponent>(static_body, Core.physics_world, static_collider.box_shape_.get(), static_transform, 0.f);
    }
  }

  ModelComponent cube_model("../../assets/grassblock.gltf"); 
  BoxColliderComponent cube_collider(Core.physics_world, cube_model.meshes_[0].transform_.scale_ * 0.1f);

  for (int x = 0; x < 2; ++x) {
    for (int y = 0; y < 1; ++y) {
      for (int z = 0; z < 2; ++z) {
        entt::entity cube = Core.registry_.create();

        TransformComponent cube_transform;
        cube_transform.position_ = glm::vec3((x * 0.2f) + 1.2f, (y * 0.2f) + 1.f, (z * 0.2f) - 3.f);
        cube_transform.scale_ = glm::vec3(0.1f, 0.1f, 0.1f);

        Core.registry_.emplace<TransformComponent>(cube, cube_transform);
        Core.registry_.emplace<ModelComponent>(cube, cube_model);
        Core.registry_.emplace<ShaderComponent>(cube, shader);
        Core.registry_.emplace<BoxColliderComponent>(cube, cube_collider);
        Core.registry_.emplace<RigidBodyComponent>(cube, Core.physics_world, cube_collider.box_shape_.get(), cube_transform, 0.3f);
      }
    }
  }

  Core.ball_model_ = ModelComponent("../../assets/ball.gltf");
  Core.sphere_component_ = SphereColliderComponent(Core.physics_world, 0.1f);

  Core.camera_ = Core.registry_.create();

  CameraComponent camera_component{};
  camera_component.near_ = 0.01f;
  camera_component.far_ = 100.f;
  camera_component.fov_ = 90.f;
  camera_component.position_ = glm::vec3(0.f, 2.f, 10.f);
  camera_component.forward_ = glm::vec3(0.f, 0.f, -1.f);
  camera_component.up_ = glm::vec3(0.f, 1.f, 0.f);

  Core.registry_.emplace<InputComponent>(Core.camera_);
  Core.registry_.emplace<FlyCameraComponent>(Core.camera_, 0.2f, 3.0f);
  Core.registry_.emplace<CameraComponent>(Core.camera_, camera_component);

  Input::SetCursorState(Input::CursorState::kCursorStateDisabled);
}

void Update(void) {
  Core.physics_world.UpdateWorld();
  UpdatePhysicsSystem(Core.registry_);

  if (Input::IsKeyPressed(GLFW_KEY_SPACE)) {
    CameraComponent camera = Core.registry_.get<CameraComponent>(Core.camera_);

    entt::entity ball = Core.registry_.create();

    TransformComponent ball_transform;
    ball_transform.position_ = camera.position_;
    ball_transform.scale_ = glm::vec3(0.1f);

    btVector3 linear_velocity = GLM_To_BT_Vec3(camera.forward_) * 5.f;

    RigidBodyComponent sphere_body(Core.physics_world, Core.sphere_component_.sphere_shape_.get(), ball_transform, 1.f);
    sphere_body.rigid_body_->applyCentralImpulse(linear_velocity);

    Core.registry_.emplace<TransformComponent>(ball, ball_transform);
    Core.registry_.emplace<ModelComponent>(ball, Core.ball_model_);
    Core.registry_.emplace<ShaderComponent>(ball, Core.default_shader_);
    Core.registry_.emplace<SphereColliderComponent>(ball, Core.sphere_component_);
    Core.registry_.emplace<RigidBodyComponent>(ball, sphere_body); 
  }
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
    .AddSystem(Application::SystemType::kSystemUpdate, [](){ UpdateMeshComponents(Core.registry_); })
    .AddSystem(Application::SystemType::kSystemUpdate, DrawDebug)
    .AddSystem(Application::SystemType::kSystemUpdate, ImGui_Backend::Render)
    .AddSystem(Application::SystemType::kSystemEnd, [](){ ReleaseMeshResources(Core.registry_); })
    .AddSystem(Application::SystemType::kSystemEnd, ImGui_Backend::End)
    .Run();
  

  std::cout << "Project Rune!" << std::endl;
  return 0;
}