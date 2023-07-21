#ifndef SHADER_COMPONENT_H_
#define SHADER_COMPONENT_H_

#include "../Graphics/Shader.h"

struct ShaderComponent {
  std::shared_ptr<Shader> shader_;
};

#endif