#ifndef DEBUG_DRAWER_H_
#define DEBUG_DRAWER_H_

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <memory>
#include <vector>

#include "VertexArray.h"
#include "Buffer.h"
#include "Shader.h"

class DebugDrawer {
public:
  struct Mesh { 
    std::shared_ptr<VertexArray> vertex_array_;
    std::shared_ptr<Buffer> index_buffer_;
    std::shared_ptr<Buffer> vertex_buffer_;
    glm::vec3 color_;
  }; 

  struct Square {
    glm::vec3 position_;
    glm::vec3 color_;
    int life_time_;
    double timer_ = 0.0;
  };

  static void InitializeDebugDrawer(void);

  static void CreateLine(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color);
  static void CreateSquare(const glm::vec3& position, const glm::vec3& color, const int& lifetime);
  static void DrawSquares(const glm::mat4& view_projection);
  static void DrawLines(const glm::mat4& view_projection);
  static void FlushLines();
  static void FlushSquares();
private: 
  static std::vector<Mesh> lines_;
  static std::vector<Square> squares_;

  static Mesh square_;
  static std::shared_ptr<Shader> shader_;
};

#endif