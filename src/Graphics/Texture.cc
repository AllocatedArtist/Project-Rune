#include "Texture.h"

#include <glad/glad.h>
#include <plog/Log.h>

#include <stb/stb_image.h>


Texture::~Texture() {
  PLOGD << "Texture deleted";
  glDeleteTextures(1, &texture_);
}

Texture& Texture::Create() {
  glGenTextures(1, &texture_);
  glBindTexture(GL_TEXTURE_2D, texture_);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  PLOGD << "Texture created";
 
  return *this;
}

void Texture::Load(const char* filename, const bool& flip) {
  glBindTexture(GL_TEXTURE_2D, texture_);

  int width = 0, height = 0, components = 0;

  stbi_set_flip_vertically_on_load(flip);
  GLubyte* data = stbi_load(filename, &width, &height, &components, 0);

  GLenum format = GL_RGB;

  if (components == 1)
    format = GL_RED;
  else if (components == 2)
    format = GL_RG;
  else if (components == 3) 
    format = GL_RGB;
  else if (components == 4)
    format = GL_RGBA;
  
  PLOG_ERROR_IF(!data) << "Unable to load " << filename;
  PLOG_ERROR_IF(!data) << stbi_failure_reason();

  assert(data && "Unable to load texture");

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data);

  stbi_image_free(data);

  PLOG_DEBUG << "Loaded texture";
}

void Texture::BindSlot(const int& slot) {
  glActiveTexture(GL_TEXTURE0 + slot);
  glBindTexture(GL_TEXTURE_2D, texture_);
}

void Texture::Unbind() {
  glBindTexture(GL_TEXTURE_2D, 0);
}




