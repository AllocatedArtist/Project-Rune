#ifndef TEXTURE_COMPONENT_H_
#define TEXTURE_COMPONENT_H_

#include <memory>

#include "../Graphics/Texture.h"

struct TextureComponent {
  std::shared_ptr<Texture> texture_;
};

#endif