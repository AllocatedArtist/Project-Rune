#include <glad/glad.h>
#include <plog/Log.h>

#include <iostream>

#include "Core/Application.h"
#include "Core/Time.h"
#include "Core/Input.h"

#include "Graphics/VertexArray.h"
#include "Graphics/Buffer.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/ModelLoader.h"

#include "Components/ModelComponent.h"
#include "Components/MeshComponent.h"
#include "Components/TextureComponent.h"
#include "Components/ShaderComponent.h"
#include "Components/InputComponent.h"
#include "Components/CameraComponent.h"
#include "Components/TransformComponent.h"
#include "Components/Components.h"

#include "Gui/ImGui_Backend.h"

#include <btBulletDynamicsCommon.h>

struct {
  Application app_ = Application(1600, 1480, "Project Rune");
  entt::registry registry_;
  entt::entity camera_;
  std::vector<entt::entity> model_entity;
  std::shared_ptr<btDefaultCollisionConfiguration> physics_config = std::make_shared<btDefaultCollisionConfiguration>(); 
  std::shared_ptr<btCollisionDispatcher> physics_dispatcher = std::make_shared<btCollisionDispatcher>(physics_config.get());
  std::shared_ptr<btBroadphaseInterface> physics_broadphase = std::make_shared<btDbvtBroadphase>();
  std::shared_ptr<btSequentialImpulseConstraintSolver> physics_solver = std::make_shared<btSequentialImpulseConstraintSolver>(); 
  std::shared_ptr<btDiscreteDynamicsWorld> physics_world = std::make_shared<btDiscreteDynamicsWorld>(physics_dispatcher.get(), physics_broadphase.get(), physics_solver.get(), physics_config.get());
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

void DrawUI(void) {
  if (ImGui::Begin("My Window", nullptr, ImGuiWindowFlags_NoResize)) {
    ImGui::SetWindowSize(ImVec2(300.f, 300.f));
    ImGui::SetWindowFontScale(2.4f);
    ImGui::Text("Frames: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
    ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);
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

struct {
  std::vector<std::shared_ptr<btCollisionShape>> shapes_;
  std::vector<std::shared_ptr<btMotionState>> motion_;
  std::vector<std::shared_ptr<btRigidBody>> bodies_;

  entt::entity physics_cube;
} PhysicsDemo;

void Setup_Physics(void) {
  Core.physics_world->setGravity(btVector3(0.f, -9.8f, 0.f));

  std::shared_ptr<Shader> shader = std::make_shared<Shader>("../../assets/shader.glsl");
  shader->Bind();
  shader->LoadUniform("texture0").SetUniform_Int("texture0", 0);
  shader->LoadUniform("model").LoadUniform("viewProjection");
  shader->Unbind();

  TransformComponent floor_transform;
  floor_transform.position_ = glm::vec3(0.f, -2.f, 0.f);

  entt::entity floor_model = Core.registry_.create();
  Core.registry_.emplace<ModelComponent>(floor_model, "../../assets/simple_plane.gltf");
  Core.registry_.emplace<TransformComponent>(floor_model, floor_transform);
  Core.registry_.emplace<ShaderComponent>(floor_model, shader);

  glm::vec3 floor_scale = Core.registry_.get<ModelComponent>(floor_model).meshes_[0].transform_.scale_;

  btTransform ground_transform;
  ground_transform.setIdentity();
  ground_transform.setOrigin(btVector3(0.f, -2.f, 0.f));

  std::shared_ptr<btCollisionShape> ground_shape = std::make_shared<btBoxShape>(btVector3(floor_scale.x, floor_scale.y, floor_scale.z));
  std::shared_ptr<btDefaultMotionState> motion_state = std::make_shared<btDefaultMotionState>(ground_transform);
  std::shared_ptr<btRigidBody> ground_body = std::make_shared<btRigidBody>(btScalar(0.f), motion_state.get(), ground_shape.get());

  TransformComponent cube_transform;
  cube_transform.position_ = glm::vec3(0.f, 10.f, 0.f);

  entt::entity physics_cube = Core.registry_.create();
  Core.registry_.emplace<ModelComponent>(physics_cube, "../../assets/grassblock.gltf");
  Core.registry_.emplace<TransformComponent>(physics_cube, cube_transform);
  Core.registry_.emplace<ShaderComponent>(physics_cube, shader);

  PhysicsDemo.physics_cube = physics_cube;

  btTransform physics_cube_transform;
  physics_cube_transform.setIdentity();
  physics_cube_transform.setOrigin(btVector3(0.f, 10.f, 0.f));

  btScalar cube_mass(0.3f);

  std::shared_ptr<btCollisionShape> box_shape = std::make_shared<btBoxShape>(btVector3(1.f, 1.f, 1.f));

  btVector3 physics_cube_inertia;
  box_shape->calculateLocalInertia(cube_mass, physics_cube_inertia);

  std::shared_ptr<btDefaultMotionState> box_motion_state = std::make_shared<btDefaultMotionState>(physics_cube_transform);
  std::shared_ptr<btRigidBody> box_body = std::make_shared<btRigidBody>(cube_mass, box_motion_state.get(), box_shape.get(), physics_cube_inertia);

  PhysicsDemo.shapes_.push_back(ground_shape);
  PhysicsDemo.shapes_.push_back(box_shape);

  PhysicsDemo.motion_.push_back(motion_state);
  PhysicsDemo.motion_.push_back(box_motion_state);

  PhysicsDemo.bodies_.push_back(ground_body);
  PhysicsDemo.bodies_.push_back(box_body);

  Core.physics_world->addRigidBody(ground_body.get());
  Core.physics_world->addRigidBody(box_body.get());

  CameraComponent camera_component{};
  camera_component.near_ = 0.01f;
  camera_component.far_ = 100.f;
  camera_component.fov_ = 90.f;
  camera_component.position_ = glm::vec3(0.f, 2.f, 4.f);
  camera_component.forward_ = glm::vec3(0.f, 0.f, -1.f);
  camera_component.up_ = glm::vec3(0.f, 1.f, 0.f);

  Core.registry_.emplace<InputComponent>(Core.camera_);
  Core.registry_.emplace<FlyCameraComponent>(Core.camera_, 0.2f, 1.0f);
  Core.registry_.emplace<CameraComponent>(Core.camera_, camera_component);

  Input::SetCursorState(Input::CursorState::kCursorStateDisabled);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
}

void Update_Physics(void) {
  Core.physics_world->stepSimulation(1.f / 60.f, 10);

  TransformComponent& t = Core.registry_.get<TransformComponent>(PhysicsDemo.physics_cube);
  btTransform transform;
  PhysicsDemo.motion_[1]->getWorldTransform(transform);
  t.position_.x = transform.getOrigin().getX();
  t.position_.y = transform.getOrigin().getY();
  t.position_.z = transform.getOrigin().getZ();
}

int main(void) {

  Application::SetTargetFPS(120);

  Core.app_
    .AddSystem(Application::SystemType::kSystemStart, ImGui_Backend::Start)
    .AddSystem(Application::SystemType::kSystemUpdate, ImGui_Backend::NewFrame)
    .AddSystem(Application::SystemType::kSystemStart, Setup_Physics)
    .AddSystem(Application::SystemType::kSystemUpdate, Update_Physics)
    .AddSystem(Application::SystemType::kSystemUpdate, DrawUI)
    .AddSystem(Application::SystemType::kSystemUpdate, ClearBackgroundColor)
    .AddSystem(Application::SystemType::kSystemUpdate, [](){ 
      UpdateCameraComponents(Core.registry_, glm::vec2(Core.app_.GetWindowWidth(), Core.app_.GetWindowHeight())); 
    })
    .AddSystem(Application::SystemType::kSystemUpdate, [](){ UpdateMeshComponents(Core.registry_); })
    .AddSystem(Application::SystemType::kSystemUpdate, ImGui_Backend::Render)
    .AddSystem(Application::SystemType::kSystemEnd, [](){ ReleaseMeshResources(Core.registry_); })
    .AddSystem(Application::SystemType::kSystemEnd, ImGui_Backend::End)
    .Run();
  

  std::cout << "Project Rune!" << std::endl;
  return 0;
}