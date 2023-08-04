#include "DebugDrawer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <plog/Log.h>
#include <glad/glad.h>

#include <algorithm>

#include "../Core/Time.h"


int DebugDrawer::current_line_batch_ = 0;
std::vector<DebugDrawer::LineBatch> DebugDrawer::line_batches_;

std::vector<DebugDrawer::Square> DebugDrawer::squares_;

DebugDrawer::Mesh DebugDrawer::square_;
std::shared_ptr<Shader> DebugDrawer::shader_;

constexpr int kMaxLineVertices = 20000 * 2;

void DebugDrawer::InitializeDebugDrawer(void) {
 const char* glsl_source = {
    "#vertex\n" 
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "uniform mat4 model;\n"
    "uniform mat4 viewProjection;\n"
    "out vec3 color;\n"
    "void main() {\n"
    "color = aColor;\n"
    "gl_Position = viewProjection * model * vec4(aPos, 1.0);\n"
    "}\n"
    "\n"
    "#fragment\n"
    "#version 330 core\n"
    "out vec4 fragColor;\n"
    "in vec3 color;\n"
    "void main() {\n"
    "fragColor = vec4(color, 1.0);\n"
    "}\n"
  };

  shader_ = std::make_shared<Shader>();
  shader_->LoadSource(glsl_source);
  shader_->LoadUniform("viewProjection").LoadUniform("model");

  std::shared_ptr<VertexArray> line_vertex_array = std::make_shared<VertexArray>();
  line_vertex_array->Create();

  std::shared_ptr<Buffer> line_vertex_buffer = std::make_shared<Buffer>(BufferType::kBufferTypeVertex);
  line_vertex_buffer->BufferData(sizeof(DebugDrawer::LineVertex) * kMaxLineVertices, nullptr, BufferUsageType::kBufferDynamic);

  line_vertex_array->VertexAttribute(0, VertexFormat::kVertexFormatFloat3, sizeof(DebugDrawer::LineVertex), (void*)offsetof(DebugDrawer::LineVertex, position_));
  line_vertex_array->VertexAttribute(1, VertexFormat::kVertexFormatFloat3, sizeof(DebugDrawer::LineVertex), (void*)offsetof(DebugDrawer::LineVertex, color_));

  line_vertex_array->Unbind();
  line_vertex_buffer->Unbind();

  line_batches_.push_back(LineBatch { line_vertex_array, line_vertex_buffer });

  PLOGD << "Initialized Debug Drawer";
}

void DebugDrawer::CreateLine(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color) {
  if (line_batches_[current_line_batch_].line_vertices_.size() >= kMaxLineVertices) {
    std::shared_ptr<VertexArray> line_vertex_array = std::make_shared<VertexArray>();
    line_vertex_array->Create();

    std::shared_ptr<Buffer> line_vertex_buffer = std::make_shared<Buffer>(BufferType::kBufferTypeVertex);
    line_vertex_buffer->BufferData(sizeof(DebugDrawer::LineVertex) * kMaxLineVertices, nullptr, BufferUsageType::kBufferDynamic);

    line_vertex_array->VertexAttribute(0, VertexFormat::kVertexFormatFloat3, sizeof(DebugDrawer::LineVertex), (void*)offsetof(DebugDrawer::LineVertex, position_));
    line_vertex_array->VertexAttribute(1, VertexFormat::kVertexFormatFloat3, sizeof(DebugDrawer::LineVertex), (void*)offsetof(DebugDrawer::LineVertex, color_));

    line_batches_.push_back(LineBatch { line_vertex_array, line_vertex_buffer });
    ++current_line_batch_;
  }
  LineBatch& current_batch = line_batches_[current_line_batch_];
  current_batch.line_vertices_.push_back(DebugDrawer::LineVertex { from, color });  
  current_batch.line_vertices_.push_back(DebugDrawer::LineVertex { to, color });  
}

void DebugDrawer::DrawLines(const glm::mat4& view_projection) { 
  for (LineBatch& current_batch : line_batches_) {
    current_batch.vertex_buffer_->Bind();

    if (current_batch.line_vertices_.size() >= 2) {
      current_batch.vertex_buffer_->BufferSubData(0, sizeof(LineVertex) * current_batch.line_vertices_.size(), &current_batch.line_vertices_[0]);
    } else {
      continue;
    }

    current_batch.vertex_buffer_->Unbind();

    current_batch.vertex_array_->Bind();
    shader_->Bind();  

    shader_->SetUniform_Matrix("model", glm::translate(glm::mat4(1.f), glm::vec3(0.f)));
    shader_->SetUniform_Matrix("viewProjection", view_projection);

    glDrawArrays(GL_LINES, 0, current_batch.line_vertices_.size());

    shader_->Unbind();
    current_batch.vertex_array_->Unbind();
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
}

void DebugDrawer::FlushLines() {
  current_line_batch_ = 0;
  for (LineBatch& batch : line_batches_) {
    batch.line_vertices_.clear();
  }
}

void DebugDrawer::FlushSquares() {
  squares_.clear();
}