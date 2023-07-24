#include "ModelLoader.h"

#include <plog/Log.h>

static void LogPrimitiveMode(const int& mode) {
  if (mode == 0)
    PLOGD << "MODE: POINTS";
  else if (mode == 1)
    PLOGD << "MODE: LINES";
  else if (mode == 2)
    PLOGD << "MODE: LINE LOOP";
  else if (mode == 3)
    PLOGD << "MODE: LINE STRIP";
  else if (mode == 4)
    PLOGD << "MODE: TRIANGLES";
  else if (mode == 5)
    PLOGD << "MODE: TRIANGLE STRIP";
  else if (mode == 6)
    PLOGD << "MODE: TRIANGLE FAN";
  else 
    PLOGD << "UNKNOWN: " << mode;
}

static void LogComponentType(const int& mode) {
  if (mode == 5120)
    PLOGD << "COMPONENT TYPE: BYTE";
  else if (mode == 5121)
    PLOGD << "COMPONENT TYPE: UNSIGNED BYTE";
  else if (mode == 5122)
    PLOGD << "COMPONENT TYPE: SHORT";
  else if (mode == 5123)
    PLOGD << "COMPONENT TYPE: UNSIGNED SHORT";
  else if (mode == 5125)
    PLOGD << "COMPONENT TYPE: UNSIGNED INT";
  else if (mode == 5126)
    PLOGD << "COMPONENT TYPE: FLOAT";
  else 
    PLOGD << "UNKNOWN: " << mode;
}

static void LogType(const int& type) {
  switch (type) {
    case TINYGLTF_TYPE_MAT2:
      PLOGD << "TYPE: MAT2";
      break;
    case TINYGLTF_TYPE_MAT3:
      PLOGD << "TYPE: MAT3";
      break;
    case TINYGLTF_TYPE_MAT4:
      PLOGD << "TYPE: MAT4";
      break;
    case TINYGLTF_TYPE_MATRIX:
      PLOGD << "TYPE: MATRIX";
      break;
    case TINYGLTF_TYPE_SCALAR:
      PLOGD << "TYPE: SCALAR";
      break;
    case TINYGLTF_TYPE_VEC2:
      PLOGD << "TYPE: VEC2";
      break;
    case TINYGLTF_TYPE_VEC3:
      PLOGD << "TYPE: VEC3";
      break;
    case TINYGLTF_TYPE_VEC4:
      PLOGD << "TYPE: VEC4";
      break;
    case TINYGLTF_TYPE_VECTOR:
      PLOGD << "TYPE: VECTOR";
  }
}

static void LogDrawTarget(const int& target) {
  if (target == 34962)
    PLOGD << "TARGET: ARRAY BUFFER";
  else if (target == 34963)
    PLOGD << "TARGET: ELEMENT ARRAY BUFFER";
}

void Model::ProcessNodes(const tinygltf::Node& node, const tinygltf::Model& model) { 

  Mesh new_mesh;
  new_mesh.primitives_ = ProcessMesh(model.meshes[node.mesh], model);
  
  new_mesh.transform_.position_ = glm::vec3(node.translation[0], node.translation[1], node.translation[2]);
  PLOGD << node.rotation.size();
  if (node.rotation.size() != 0.f) {
    assert(node.rotation.size() == 4);
    new_mesh.transform_.rotation_ = glm::quat(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]);
  }
  new_mesh.transform_.scale_ = glm::vec3(node.scale[0], node.scale[1], node.scale[2]);

  meshes_.push_back(new_mesh);

  for (const int& child : node.children) {
    ProcessNodes(model.nodes[child], model);
  }
}

std::vector<Primitive> Model::ProcessMesh(const tinygltf::Mesh& mesh, const tinygltf::Model& model) {
  PLOGD << mesh.name;
  std::vector<PrimitiveData> primitives;
  for (const tinygltf::Primitive& primitive : mesh.primitives) {
    PrimitiveData primitive_data;
    LogPrimitiveMode(primitive.mode);
    primitive_data.draw_mode_ = primitive.mode;
    const tinygltf::Accessor& index_accessor = model.accessors[primitive.indices];
    PLOGD_IF(!index_accessor.name.empty()) << index_accessor.name;
    PLOGD << "INDEX ACCESSOR BYTE OFFSET: " << index_accessor.byteOffset;
    LogComponentType(index_accessor.componentType);
    primitive_data.component_type_ = index_accessor.componentType;
    PLOGD << "INDEX ACCESSOR COUNT: " << index_accessor.count;
    primitive_data.indices_count_ = index_accessor.count;
    LogType(index_accessor.type);
    
    const tinygltf::BufferView* index_buffer_view = &model.bufferViews[index_accessor.bufferView] + index_accessor.byteOffset;
    const tinygltf::Buffer& index_buffer = model.buffers[index_buffer_view->buffer];

    PLOGD << "INDEX DATA SIZE: " << index_buffer_view->byteLength;
    PLOGD << "INDEX DATA OFFSET: " << index_buffer_view->byteOffset;

    PLOGD << "INDEX DATA STRIDE: " << index_buffer_view->byteStride;

    assert(index_buffer_view->byteStride == 0 && "Byte stride of indices is not 0!");

    auto indices_slice_begin = index_buffer.data.cbegin() + index_buffer_view->byteOffset;
    auto indices_slice_end = indices_slice_begin + index_buffer_view->byteLength;

    primitive_data.indices_ = std::vector<unsigned char>(indices_slice_begin, indices_slice_end);

    for (const auto& attribute : primitive.attributes) {
      PLOGD << "ATTRIBUTE: " << attribute.first;
      const tinygltf::Accessor& accessor = model.accessors[attribute.second];
      PLOG_DEBUG_IF(!accessor.name.empty()) << accessor.name;
      LogComponentType(accessor.componentType);
      LogType(accessor.type);
      PLOGD << "COUNT: " << accessor.count;

      PLOGD << "ACCESSOR BYTE OFFSET: " << accessor.byteOffset;
      const tinygltf::BufferView* buffer_view = &model.bufferViews[accessor.bufferView] + accessor.byteOffset;
      PLOGD << "ACCESSOR BYTE STRIDE: " << buffer_view->byteStride;
      assert(buffer_view->byteStride == 0 && "Byte stride is not 0!");
      LogDrawTarget(buffer_view->target);

      const tinygltf::Buffer& buffer = model.buffers[buffer_view->buffer];
      auto data_begin = buffer.data.cbegin() + buffer_view->byteOffset;
      auto data_end = data_begin + buffer_view->byteLength;

      if (attribute.first == "POSITION") {
        primitive_data.position_ = std::vector<unsigned char>(data_begin, data_end);
      } else if (attribute.first == "TEXCOORD_0") {
        primitive_data.texcoords_ = std::vector<unsigned char>(data_begin, data_end);
      }
    }
    assert(!primitive_data.indices_.empty() && "NO INDICES COLLECTED");
    assert(!primitive_data.position_.empty() && "NO POSITION DATA COLLECTED");
    assert(!primitive_data.texcoords_.empty() && "NO TEXCOORD DATA COLLECTED");

    PLOGD << "---PRIMITIVE---";
    LogComponentType(primitive_data.component_type_);
    LogPrimitiveMode(primitive_data.draw_mode_);
    PLOGD << primitive_data.indices_count_;
    PLOGD << primitive_data.position_.size();
    PLOGD << primitive_data.texcoords_.size();
    PLOGD << primitive_data.indices_.size();
    PLOGD << "------END------";

    primitives.push_back(primitive_data); 
  }

  std::vector<Primitive> processed_primitives;

  for (PrimitiveData& data : primitives) {
    Primitive primitive;

    primitive.vertex_array_ = std::make_shared<VertexArray>();
    primitive.vertex_array_->Create();
    primitive.vertex_buffer_ = std::make_shared<Buffer>(BufferType::kBufferTypeVertex);
    primitive.index_buffer_ = std::make_shared<Buffer>(BufferType::kBufferTypeIndex);

    primitive.vertex_buffer_->BufferData(data.position_.size() + data.texcoords_.size(), nullptr);
    primitive.vertex_buffer_->BufferSubData(0, data.position_.size(), &data.position_[0]);
    primitive.vertex_buffer_->BufferSubData(data.position_.size(), data.texcoords_.size(), &data.texcoords_[0]);

    primitive.index_buffer_->BufferData(data.indices_.size(), &data.indices_[0]);

    primitive.vertex_array_->VertexAttribute(0, VertexFormat::kVertexFormatFloat3, sizeof(float) * 3, 0);
    primitive.vertex_array_->VertexAttribute(1, VertexFormat::kVertexFormatFloat2, sizeof(float) * 2, (void*)(data.position_.size()));

    primitive.vertex_array_->Unbind();
    primitive.vertex_buffer_->Unbind();
    primitive.index_buffer_->Unbind();

    primitive.draw_mode_ = data.draw_mode_; 
    primitive.component_type_ = data.component_type_;
    primitive.indices_count_ = data.indices_count_;

    processed_primitives.push_back(primitive);
  }

  assert(processed_primitives.size() != 0 && "No mesh primitives processed!");
  return processed_primitives;
}

void Model::LoadModel(const std::string& filename) {
  tinygltf::TinyGLTF loader;
  tinygltf::Model model;
  std::string warning, error;
  loader.LoadASCIIFromFile(&model, &error, &warning, filename);

  PLOG_WARNING_IF(!warning.empty()) << warning;
  PLOG_ERROR_IF(!error.empty()) << error;

  for (const int& node : model.scenes[model.defaultScene].nodes) {
    ProcessNodes(model.nodes[node], model);
  }
}

std::vector<Mesh>& Model::GetMeshes() {
  return meshes_;
}