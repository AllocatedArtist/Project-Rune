#ifndef MESH_COMPONENT_H_
#define MESH_COMPONENT_H_

#include "../Graphics/VertexArray.h"
#include "../Graphics/Buffer.h"
#include "TransformComponent.h"

#include <memory>

constexpr int kComponentType_UnsignedShort = 5123;
constexpr int kDrawMode_Triangle = 4;

struct MeshComponent {
  std::shared_ptr<VertexArray> vertex_array_;
  std::shared_ptr<Buffer> vertex_buffer_;
  std::shared_ptr<Buffer> index_buffer_;

  int num_vertices_;
  int num_indices_;

  int index_type_ = kComponentType_UnsignedShort;  
  int draw_mode_ = kDrawMode_Triangle;

  TransformComponent local_transform_;
};


#endif