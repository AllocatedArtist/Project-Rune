#include <glad/glad.h>
#include <plog/Log.h>

#include <iostream>

#include "Core/Application.h"
#include "Core/Input.h"

#include "Graphics/VertexArray.h"
#include "Graphics/Buffer.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"

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
    0, 1, 2,
    2, 3, 0,
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
  texture->BindSlot(0);
  shader->Bind();
  shader->LoadUniform("texture0").SetUniform_Int("texture0", 0);
  shader->LoadUniform("model");
  shader->LoadUniform("viewProjection");

  shader->Unbind();
  texture->Unbind();

  Core.textured_rec_ = Core.registry_.create();
  Core.registry_.emplace<MeshComponent>(Core.textured_rec_, vao, vbo, ebo, 4, 6);
  Core.registry_.emplace<TextureComponent>(Core.textured_rec_, texture);
  Core.registry_.emplace<ShaderComponent>(Core.textured_rec_, shader); 

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
  Core.registry_.emplace<FlyCameraComponent>(Core.camera_, 0.1f, 1.0f);
  Core.registry_.emplace<CameraComponent>(Core.camera_, camera_component);

  Input::SetCursorState(Input::CursorState::kCursorStateDisabled);
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
  glClear(GL_COLOR_BUFFER_BIT);
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

int main(void) {
  Core.app_
    .AddSystem(Application::SystemType::kSystemStart, ImGui_Backend::Start)
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