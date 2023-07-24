#include <glad/glad.h>
#include <plog/Log.h>

#include <iostream>

#include "Core/Application.h"
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


struct {
  Application app_ = Application(1600, 1480, "Project Rune");
  entt::registry registry_;
  entt::entity textured_rec_;
  entt::entity camera_;
  std::vector<entt::entity> model_entity;
  Model model;
} Core;

void Setup_Buffers() {
  PLOGD << "SETUP!";
  float vertices[] = {
    0.5, 0.5, 0.0, 1.0, 1.0, 
    0.5, -0.5, 0.0, 1.0, 0.0,
    -0.5, -0.5, 0.0, 0.0, 0.0,
    -0.5, 0.5, 0.0, 0.0, 1.0,
  };

  unsigned short indices[] = {
    2, 1, 0,
    3, 2, 0,
  };

  std::shared_ptr<VertexArray> vao = std::make_shared<VertexArray>();
  vao->Create();

  std::shared_ptr<Buffer> vbo = std::make_shared<Buffer>(BufferType::kBufferTypeVertex);
  std::shared_ptr<Buffer> ebo = std::make_shared<Buffer>(BufferType::kBufferTypeIndex);

  vbo->BufferData(sizeof(vertices), vertices);
  ebo->BufferData(sizeof(indices), indices);

  vao->VertexAttribute(0, VertexFormat::kVertexFormatFloat3, sizeof(float) * 5, 0);
  vao->VertexAttribute(1, VertexFormat::kVertexFormatFloat2, sizeof(float) * 5, (void*)(sizeof(float) * 3));

  vao->Unbind();
  vbo->Unbind();
  ebo->Unbind();

  std::shared_ptr<Texture> texture = std::make_shared<Texture>();
  texture->Create().Load("../../assets/smiley.png", true);

  std::shared_ptr<Shader> shader = std::make_shared<Shader>("../../assets/shader.glsl");
  shader->Bind();
  shader->LoadUniform("texture0").SetUniform_Int("texture0", 0);
  shader->LoadUniform("model");
  shader->LoadUniform("viewProjection");

  shader->Unbind();

  Core.textured_rec_ = Core.registry_.create();
  Core.registry_.emplace<MeshComponent>(Core.textured_rec_, vao, vbo, ebo, 4, 6);
  Core.registry_.emplace<TextureComponent>(Core.textured_rec_, texture);
  Core.registry_.emplace<ShaderComponent>(Core.textured_rec_, shader); 

  TransformComponent better_t;
  better_t.position_ = glm::vec3(0.f, 0.f, -5.f);

  TransformComponent grass_t;
  grass_t.position_ = glm::vec3(2.f, 1.f, -0.5f);
  grass_t.scale_ = glm::vec3(0.3f, 0.3f, 0.3f);


  entt::entity e = Core.registry_.create();
  Core.registry_.emplace<ModelComponent>(e, "../../assets/better.gltf");
  Core.registry_.emplace<TransformComponent>(e, better_t);
  Core.registry_.emplace<ShaderComponent>(e, shader);

  Core.model_entity.push_back(e);

  entt::entity e2 = Core.registry_.create();
  Core.registry_.emplace<ModelComponent>(e2, "../../assets/grassblock.gltf");
  Core.registry_.emplace<TransformComponent>(e2, grass_t);
  Core.registry_.emplace<ShaderComponent>(e2, shader);

  Core.model_entity.push_back(e2);

  

  TransformComponent transform;
  transform.position_ = glm::vec3(0.0, 0.0, 0.0);
  transform.rotation_ = glm::angleAxis(glm::radians(-55.f), glm::vec3(1.f, 0.f, 0.f));
  transform.scale_ = glm::vec3(1.0);

  Core.registry_.emplace<TransformComponent>(Core.textured_rec_, transform);

  Core.camera_ = Core.registry_.create();

  CameraComponent camera_component{};
  camera_component.near_ = 0.01f;
  camera_component.far_ = 100.f;
  camera_component.fov_ = 90.f;
  camera_component.position_ = glm::vec3(0.f, 0.f, 1.f);
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
    t.rotation_ = glm::rotate(t.rotation_, (i % 2 == 0) ? Application::GetDeltaTime() : -Application::GetDeltaTime(), glm::vec3(1.f, 1.f, 0.f));
  }
}

int main(void) {
  Core.app_
    .AddSystem(Application::SystemType::kSystemStart, ImGui_Backend::Start)
    .AddSystem(Application::SystemType::kSystemStart, [](){ Core.model.LoadModel("../../assets/better.gltf"); PLOGD << "MODEL LOADED"; })
    .AddSystem(Application::SystemType::kSystemStart, Setup_Buffers)
    .AddSystem(Application::SystemType::kSystemUpdate, ImGui_Backend::NewFrame)
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