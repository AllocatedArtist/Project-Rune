#include "VertexArray.h"

#include <glad/glad.h>
#include <plog/Log.h>

void VertexArray::Create() {
  glGenVertexArrays(1, &id_);
  glBindVertexArray(id_);

  PLOGD << "Created vertex array";
}

VertexArray::~VertexArray() {
  PLOGD << "Destroyed vertex array";
  glDeleteVertexArrays(1, &id_);
}

void VertexArray::Bind() {
  glBindVertexArray(id_);
}

void VertexArray::Unbind() {
  glBindVertexArray(0);
}

void VertexArray::VertexAttribute(const unsigned int& index, const VertexFormat& format, const unsigned long long& stride, void* offset) {

  int num_components = 0;
  GLenum type = 0;

  switch (format) {
    case VertexFormat::kVertexFormatFloat2:
      type = GL_FLOAT;
      num_components = 2;
      break;
    case VertexFormat::kVertexFormatFloat3:
      type = GL_FLOAT;
      num_components = 3; 
      break;
  }

  glVertexAttribPointer(index, num_components, type, GL_FALSE, stride, offset);
  glEnableVertexAttribArray(index);
}
