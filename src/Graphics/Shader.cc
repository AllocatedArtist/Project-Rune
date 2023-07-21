#include "Shader.h"

#include <glad/glad.h>
#include <plog/Log.h>

#include <string>
#include <string_view>
#include <fstream>
#include <filesystem>

#include <glm/gtc/type_ptr.hpp>

static std::tuple<std::string, std::string> LoadGLSL(const std::string_view& filename) {
  std::ifstream glsl_file(filename.data());
  std::string file_contents = std::string(std::istreambuf_iterator<char>(glsl_file), std::istreambuf_iterator<char>());

  size_t identifier_vertex = file_contents.find("#vertex");
  size_t identifier_fragment = file_contents.find("#fragment");

  constexpr size_t vertex_identifier_length = 8;
  constexpr size_t fragment_identifier_length = 10;

  auto vertex_begin = file_contents.begin() + identifier_vertex + vertex_identifier_length;
  auto fragment_begin = file_contents.begin() + identifier_fragment + fragment_identifier_length;

  auto vertex_end = vertex_begin + (identifier_fragment - 8);
  auto fragment_end = file_contents.end();
 
  std::string vertex_source(vertex_begin, vertex_end);
  std::string fragment_source(fragment_begin, fragment_end);

  return std::make_tuple(vertex_source, fragment_source);
}

static bool CheckShaderCompileStatus(const unsigned int& shader) {
  int success = 0; 
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char log[512];
    glGetShaderInfoLog(shader, 512, nullptr, log);
    PLOG_ERROR << log;
    return false;
  }

  return true;
}

static bool CheckProgramLinkStatus(const unsigned int& program) {
  int success = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &success);

  if (!success) {
    char log[512];
    glGetProgramInfoLog(program, 512, nullptr, log);
    PLOG_ERROR << log;
    return false;
  }

  return true;
}

Shader::Shader(const char* filename) {
  program_id_ = glCreateProgram();
  vertex_shader_id_ = glCreateShader(GL_VERTEX_SHADER); 
  fragment_shader_id_ = glCreateShader(GL_FRAGMENT_SHADER);

  std::filesystem::path shader_path(filename);

  PLOG_WARNING_IF(!std::filesystem::exists(shader_path)) << "Unable to find " << filename;
  assert(std::filesystem::exists(shader_path) && "Could not find shader file");

  auto [vertex_string, fragment_string] = LoadGLSL(filename); 
  const char* vertex_cstr = vertex_string.c_str();
  const char* fragment_cstr = fragment_string.c_str();

  glShaderSource(vertex_shader_id_, 1, &vertex_cstr, nullptr);
  glShaderSource(fragment_shader_id_, 1, &fragment_cstr, nullptr);

  glCompileShader(vertex_shader_id_);

  PLOG_ERROR_IF(!CheckShaderCompileStatus(vertex_shader_id_)) << "Error loading vertex shader";
  assert(CheckShaderCompileStatus(vertex_shader_id_));

  glCompileShader(fragment_shader_id_);

  PLOG_ERROR_IF(!CheckShaderCompileStatus(fragment_shader_id_)) << "Error loading fragment shader";
  assert(CheckShaderCompileStatus(fragment_shader_id_));

  glAttachShader(program_id_, vertex_shader_id_);
  glAttachShader(program_id_, fragment_shader_id_);

  glLinkProgram(program_id_);

  glDeleteShader(vertex_shader_id_);
  glDeleteShader(fragment_shader_id_);


  PLOG_ERROR_IF(!CheckProgramLinkStatus(program_id_)) << "Error linking program";
  assert(CheckProgramLinkStatus(program_id_));

  PLOGD << "Created shader successfully";
}

Shader::~Shader() {
  PLOGD << "Deleted shader";
  glDeleteProgram(program_id_);
}

void Shader::Bind() {
  glUseProgram(program_id_);
}

void Shader::Unbind() {
  glUseProgram(0);
}

Shader& Shader::LoadUniform(const char* uniform) {
  uniforms_.try_emplace(uniform, glGetUniformLocation(program_id_, uniform));
  return *this;
}

void Shader::SetUniform_Int(const char* uniform, const int& value) {
  glUniform1i(uniforms_[uniform], value);
}

void Shader::SetUniform_Float(const char* uniform, const float& value) {
  glUniform1f(uniforms_[uniform], value);
}

void Shader::SetUniform_Float2(const char* uniform, const float& x, const float& y) {
  glUniform2f(uniforms_[uniform], x, y);
}

void Shader::SetUniform_Float3(const char* uniform, const float& x, const float& y, const float& z) {
  glUniform3f(uniforms_[uniform], x, y, z);
}

void Shader::SetUniform_Matrix(const char* uniform,  const glm::mat4& matrix) {
  glUniformMatrix4fv(uniforms_[uniform], 1, GL_FALSE, glm::value_ptr(matrix));
}
