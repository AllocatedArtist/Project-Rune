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

#include "Components/Components.h"

struct {
  entt::registry registry_;
  entt::entity textured_rec_;
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
  texture->Create().Load("assets/smiley.png", true);

  std::shared_ptr<Shader> shader = std::make_shared<Shader>("assets/shader.glsl");
  texture->BindSlot(0);
  shader->Bind();
  shader->LoadUniform("texture0").SetUniform_Int("texture0", 0);

  shader->Unbind();
  texture->Unbind();

  Core.textured_rec_ = Core.registry_.create();
  Core.registry_.emplace<MeshComponent>(Core.textured_rec_, vao, vbo, ebo, 4, 6);
  Core.registry_.emplace<TextureComponent>(Core.textured_rec_, texture);
  Core.registry_.emplace<ShaderComponent>(Core.textured_rec_, shader); 
}

void ClearBackgroundColor(void) {
  glClear(GL_COLOR_BUFFER_BIT);
  glClearColor(0.95, 0.75, 0.75, 1.0);
}

int main(void) {
  Application app(1600, 1480, "Project Rune");

  app
    .AddSystem(Application::SystemType::kSystemStart, Setup_Buffers)
    .AddSystem(Application::SystemType::kSystemUpdate, ClearBackgroundColor)
    .AddSystem(Application::SystemType::kSystemUpdate, [](){ UpdateMeshComponents(Core.registry_); })
    .AddSystem(Application::SystemType::kSystemEnd, [](){ ReleaseMeshResources(Core.registry_); })
    .Run();

  std::cout << "Project Rune!" << std::endl;
  return 0;
}