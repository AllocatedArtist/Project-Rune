#include "DebugDrawer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <plog/Log.h>
#include <glad/glad.h>

#include <algorithm>

#include "../Core/Time.h"

std::vector<DebugDrawer::Mesh> DebugDrawer::lines_;
std::vector<DebugDrawer::Square> DebugDrawer::squares_;

DebugDrawer::Mesh DebugDrawer::square_;
std::shared_ptr<Shader> DebugDrawer::shader_;

void DebugDrawer::InitializeDebugDrawer(void) {
 const char* glsl_source = {
    "#vertex\n" 
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "uniform mat4 model;\n"
    "uniform mat4 viewProjection;\n"
    "void main() {\n"
    "gl_Position = viewProjection * model * vec4(aPos, 1.0);\n"
    "}\n"
    "\n"
    "#fragment\n"
    "#version 330 core\n"
    "out vec4 fragColor;\n"
    "uniform vec3 color;\n"
    "void main() {\n"
    "fragColor = vec4(color, 1.0);\n"
    "}\n"
  };

  shader_ = std::make_shared<Shader>();
  shader_->LoadSource(glsl_source);
  shader_->LoadUniform("viewProjection").LoadUniform("model").LoadUniform("color");

  float square_vertices[] = {
    0.2f, 0.2f, 0.0f,
    0.2f, -0.2f, 0.0f,
    -0.2f, -0.2f, 0.0f,
    -0.2f, 0.2f, 0.0f,
  };

  unsigned short square_indices[] = {
    0, 1, 2,
    2, 3, 0,
  };

  square_.vertex_array_ = std::make_shared<VertexArray>();
  square_.vertex_array_->Create();

  square_.vertex_buffer_ = std::make_shared<Buffer>(BufferType::kBufferTypeVertex);
  square_.index_buffer_ = std::make_shared<Buffer>(BufferType::kBufferTypeIndex);

  square_.vertex_buffer_->BufferData(sizeof(square_vertices), square_vertices);
  square_.index_buffer_->BufferData(sizeof(square_indices), square_indices);

  square_.vertex_array_->VertexAttribute(0, VertexFormat::kVertexFormatFloat3, sizeof(float) * 3, 0);

  square_.vertex_array_->Unbind();
  square_.vertex_buffer_->Unbind();
  square_.index_buffer_->Unbind();

  PLOGD << "Initialized Debug Drawer";
}

void DebugDrawer::CreateLine(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color) {
  float vertices[] = {
    from.x, from.y, from.z,
    to.x, to.y, to.z,
  };

  Mesh line; 
  line.vertex_array_ = std::make_shared<VertexArray>();
  line.vertex_array_->Create();

  line.vertex_buffer_ = std::make_shared<Buffer>(BufferType::kBufferTypeVertex);
  line.vertex_buffer_->BufferData(sizeof(vertices), vertices);

  line.vertex_array_->VertexAttribute(0, VertexFormat::kVertexFormatFloat3, sizeof(float) * 3, 0);

  line.vertex_array_->Unbind();
  line.vertex_buffer_->Unbind();

  line.color_ = color;

  lines_.push_back(line);
}

void DebugDrawer::DrawLines(const glm::mat4& view_projection) {
  for (Mesh& line : lines_) {
    line.vertex_array_->Bind();
    shader_->Bind();

    shader_->SetUniform_Float3("color", line.color_.x, line.color_.y, line.color_.z);
    shader_->SetUniform_Matrix("viewProjection", view_projection);
    shader_->SetUniform_Matrix("model", glm::translate(glm::mat4(1.0), glm::vec3(0.f, 0.f, 0.f)));

    glDrawArrays(GL_LINES, 0, 2);

    shader_->Unbind();
    line.vertex_array_->Unbind();
  }
}

void DebugDrawer::DrawSquares(const glm::mat4& view_projection) {
  for (Square& square : squares_) {
    square_.vertex_array_->Bind();
    shader_->Bind();

    shader_->SetUniform_Float3("color", square.color_.x, square.color_.y, square.color_.z);
    shader_->SetUniform_Matrix("viewProjection", view_projection);
    shader_->SetUniform_Matrix("model", glm::translate(glm::mat4(1.0), square.position_));

    glDisable(GL_CULL_FACE);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    glEnable(GL_CULL_FACE);

    shader_->Unbind();
    square_.vertex_array_->Unbind();

    if (square.life_time_ != -1) {
      square.timer_ += Time::GetDeltaTime();
    }
  }

  std::remove_if(squares_.begin(), squares_.end(), [](const Square& square) {
    return (square.timer_ >= square.life_time_) && square.life_time_ != -1;
  });
}

void DebugDrawer::CreateSquare(const glm::vec3& position, const glm::vec3& color, const int& lifetime) {
  squares_.push_back(Square { position, color, lifetime });
}

void DebugDrawer::FlushLines() {
  for (Mesh& line : lines_) {
    line.vertex_buffer_.reset();
    line.vertex_array_.reset();
  }

  lines_.clear();
}

void DebugDrawer::FlushSquares() {
  squares_.clear();
}