#include "Buffer.h"

#include <glad/glad.h>
#include <plog/Log.h>

static GLenum BufferTypeToGL(const BufferType& type) {
  if (type == BufferType::kBufferTypeIndex)
    return GL_ELEMENT_ARRAY_BUFFER;
  return GL_ARRAY_BUFFER;
}

Buffer::Buffer(const BufferType& type) : type_(type) {
  glGenBuffers(1, &id_);
  glBindBuffer(BufferTypeToGL(type), id_);

  PLOGV << "Created buffer: " << (type == BufferType::kBufferTypeVertex ? " Vertex Buffer" : " Index Buffer"); 
}

Buffer::~Buffer() {
  PLOGV << "Deleted buffer: " << (type_ == BufferType::kBufferTypeVertex ? " Vertex Buffer" : " Index Buffer"); 
  glDeleteBuffers(1, &id_);
}

void Buffer::Bind() {
  glBindBuffer(BufferTypeToGL(type_), id_);
}

void Buffer::Unbind() {
 glBindBuffer(BufferTypeToGL(type_), 0); 
}

void Buffer::BufferData(const unsigned long long& size, const void* data) {
 glBufferData(BufferTypeToGL(type_), size, data, GL_STATIC_DRAW);
}

void Buffer::BufferSubData(const unsigned long long& offset, const unsigned int& size, const void* data) {
  glBufferSubData(BufferTypeToGL(type_), offset, size, data);
}
