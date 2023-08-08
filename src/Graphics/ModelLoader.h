#ifndef MODEL_LOADER_H_
#define MODEL_LOADER_H_

#include <tinygltf/tiny_gltf.h>

#include <memory>

#include "../Components/TransformComponent.h"

struct MaterialData {
  bool use_texture_ = false;
  int wrap_s_ = 0;
  int wrap_t_ = 0;
  int component_ = 0;
  int bits_ = 0;
  std::vector<unsigned char> texture_data_;
  int texture_width_ = 0;
  int texture_height_ = 0;
  glm::vec3 base_color_ = glm::vec3(0.f);
  std::string name_;
};

struct PrimitiveData {
  std::vector<unsigned char> position_;
  std::vector<unsigned char> normal_;
  std::vector<unsigned char> texcoords_;
  std::vector<unsigned char> indices_;
  int indices_count_;
  int draw_mode_;
  int component_type_;
  MaterialData material_;
};

struct Mesh {
  std::vector<PrimitiveData> primitives_;
  TransformComponent local_transform_; 
};

class Model {
public:
  Model() = default;
  void LoadModel(const std::string& filename);
  std::vector<Mesh>& GetMeshes();
private:
  void ProcessNodes(const tinygltf::Node& node, const tinygltf::Model& model);
  std::vector<PrimitiveData> ProcessMesh(const tinygltf::Mesh& mesh, const tinygltf::Model& model); 
private:
  std::vector<Mesh> meshes_;
};

#endif