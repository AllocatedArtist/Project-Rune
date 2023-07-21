#ifndef TEXTURE_COMPONENT_H_
#define TEXTURE_COMPONENT_H_

#include "../Graphics/Texture.h"

struct TextureComponent {
  std::shared_ptr<Texture> texture_;
};


#endif