#ifndef SHADER_H_
#define SHADER_H_

#include <map>
#include <glm/mat4x4.hpp>
#include <string>

class Shader {
public:
  Shader() = default;
  Shader(const char* filename);
  ~Shader();

  void Bind();
  void Unbind();

  Shader& LoadUniform(const std::string& uniform);

  void SetUniform_Int(const std::string& uniform, const int& value);
  void SetUniform_Float(const std::string& uniform, const float& value);
  void SetUniform_Float2(const std::string& uniform, const float& x, const float& y);
  void SetUniform_Float3(const std::string& uniform, const float& x, const float& y, const float& z);
  void SetUniform_Matrix(const std::string& uniform,  const glm::mat4& matrix);
private:
  unsigned int program_id_;
  unsigned int vertex_shader_id_;
  unsigned int fragment_shader_id_;

  std::map<std::string, int> uniforms_;
};


#endif