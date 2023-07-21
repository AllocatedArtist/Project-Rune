#ifndef MESH_COMPONENT_H_
#define MESH_COMPONENT_H_

#include "../Graphics/VertexArray.h"
#include "../Graphics/Buffer.h"

#include <memory>

struct MeshComponent {
  std::shared_ptr<VertexArray> vertex_array_;
  std::shared_ptr<Buffer> vertex_buffer_;
  std::shared_ptr<Buffer> index_buffer_;

  int num_vertices_;
  int num_indices_;
};


#endif