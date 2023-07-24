#ifndef MODEL_LOADER_H_
#define MODEL_LOADER_H_

#include <tinygltf/tiny_gltf.h>
#include <memory>

#include "../Components/TransformComponent.h"

#include "VertexArray.h"
#include "Buffer.h"


struct PrimitiveData {
  std::vector<unsigned char> position_;
  std::vector<unsigned char> texcoords_;
  std::vector<unsigned char> indices_;
  int indices_count_;
  int draw_mode_;
  int component_type_;
};

struct Primitive {
  std::shared_ptr<VertexArray> vertex_array_;
  std::shared_ptr<Buffer> vertex_buffer_;
  std::shared_ptr<Buffer> index_buffer_;
  int indices_count_;
  int draw_mode_;
  int component_type_; //GL_UNSIGNED_SHORT or GL_UNSIGNED_INT?
};

struct Mesh {
  std::vector<Primitive> primitives_;
  TransformComponent transform_; 
};

class Model {
public:
  Model() = default;
  void LoadModel(const std::string& filename);
  std::vector<Mesh>& GetMeshes();
private:
  void ProcessNodes(const tinygltf::Node& node, const tinygltf::Model& model);
  std::vector<Primitive> ProcessMesh(const tinygltf::Mesh& mesh, const tinygltf::Model& model); 
private:
  std::vector<Mesh> meshes_;
};

#endif