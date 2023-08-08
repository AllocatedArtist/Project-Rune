#include "ResourceManager.h"

#include <glad/glad.h>
#include <plog/Log.h>

#include "../Graphics/ModelLoader.h"
#include "../Graphics/Texture.h"
#include "../Graphics/Shader.h"

#include "../Components/MeshComponent.h"
#include "../Components/ShaderComponent.h"
#include "../Components/TextureComponent.h"

static MeshComponent CreateMeshComponentFromPrimitive(const PrimitiveData& primitive) {
  MeshComponent mesh_component;

  mesh_component.index_type_ = primitive.component_type_;
  mesh_component.num_indices_ = primitive.indices_count_;
  mesh_component.draw_mode_ = primitive.draw_mode_;
    
  mesh_component.vertex_array_ = std::make_shared<VertexArray>();
  mesh_component.vertex_array_->Create();

  mesh_component.vertex_buffer_ = std::make_shared<Buffer>(BufferType::kBufferTypeVertex);

  mesh_component.vertex_buffer_->BufferData(
    primitive.position_.size() + primitive.normal_.size() + primitive.texcoords_.size(), 
    nullptr, 
    BufferUsageType::kBufferStatic
  );

  mesh_component.vertex_buffer_->BufferSubData(0, primitive.position_.size(), &primitive.position_[0]);
  mesh_component.vertex_buffer_->BufferSubData(primitive.position_.size(), primitive.normal_.size(), &primitive.normal_[0]);
  mesh_component.vertex_buffer_->BufferSubData(primitive.position_.size() + primitive.normal_.size(), primitive.texcoords_.size(), &primitive.texcoords_[0]);

  mesh_component.index_buffer_ = std::make_shared<Buffer>(BufferType::kBufferTypeIndex);
  mesh_component.index_buffer_->BufferData(primitive.indices_.size(), &primitive.indices_[0], BufferUsageType::kBufferStatic);
    
  mesh_component.vertex_array_->VertexAttribute(0, VertexFormat::kVertexFormatFloat3, sizeof(float) * 3, 0);
  mesh_component.vertex_array_->VertexAttribute(1, VertexFormat::kVertexFormatFloat3, sizeof(float) * 3, (void*)(primitive.position_.size()));
  mesh_component.vertex_array_->VertexAttribute(2, VertexFormat::kVertexFormatFloat3, sizeof(float) * 2, (void*)(primitive.position_.size() + primitive.normal_.size()));

  mesh_component.vertex_array_->Unbind();
  mesh_component.vertex_buffer_->Unbind();
  mesh_component.index_buffer_->Unbind();

  PLOGD << "Created new mesh";

  return mesh_component;
}

static TextureComponent CreateTextureComponentFromMaterial(const MaterialData& material) {
  TextureComponent texture_component;
  texture_component.texture_ = std::make_shared<Texture>();
  texture_component.texture_->Create();

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, material.wrap_s_);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, material.wrap_t_);

  GLenum format = GL_RGB;
  if (material.component_ == 1) 
    format = GL_RED;
  else if (material.component_ == 2) 
    format = GL_RG;
  else if (material.component_ == 3)
    format = GL_RGB;
  else if (material.component_ == 4) 
    format = GL_RGBA;
  else
    PLOGD << "Unknown component: " << material.component_;
      
  GLenum bits = GL_UNSIGNED_BYTE;
  if (material.bits_ == 16)
    bits = GL_UNSIGNED_SHORT;
  else if (material.bits_ == 8)
    bits = GL_UNSIGNED_BYTE;
  else 
    PLOGD << "UNKNOWN BITS: " << material.bits_;

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, material.texture_width_, material.texture_height_, 0, format, bits, &material.texture_data_.at(0));
  glGenerateMipmap(GL_TEXTURE_2D); 

  texture_component.texture_->Unbind(); 

  PLOGD << "Created new texture component";

  return texture_component;
}

static entt::entity CreateMeshHandle(entt::registry& registry, std::map<std::string, MaterialComponent>& material_map, const PrimitiveData& primitive) {

  entt::entity handle = registry.create();

  registry.emplace<MeshComponent>(handle, CreateMeshComponentFromPrimitive(primitive));

  MaterialComponent material;

  if (material_map.find(primitive.material_.name_) != material_map.cend()) {
    PLOGD << "Used material cache for " << primitive.material_.name_;
    material = material_map[primitive.material_.name_];
  } else {
    material.base_color_ = primitive.material_.base_color_;

    if (primitive.material_.use_texture_) {
      material.texture_handle_ = registry.create();
      registry.emplace<TextureComponent>(material.texture_handle_, CreateTextureComponentFromMaterial(primitive.material_));  
      material_map.insert(std::make_pair(primitive.material_.name_, material));
    } 
  }

  registry.emplace<MaterialComponent>(handle, material);

  return handle;
}

void ResourceManager::LoadModelAsset(const std::string& model_path) {
  Model model;
  model.LoadModel(model_path);

  ModelResource model_resource;

  for (const Mesh& mesh : model.GetMeshes()) {
    for (const PrimitiveData& primitive : mesh.primitives_) {
      entt::entity mesh_handle = CreateMeshHandle(registry_, material_map_, primitive);
      registry_.emplace<TransformComponent>(mesh_handle, mesh.local_transform_);
      model_resource.mesh_handles_.push_back(mesh_handle);
    }
  }

  model_map_.insert_or_assign(model_path, model_resource);
  PLOGD << "Loaded model asset: " << model_path << " successfully";
}

void ResourceManager::LoadShaderAsset(const std::string& shader_path) {
  std::shared_ptr<Shader> shader = std::make_shared<Shader>(shader_path.c_str());

  entt::entity shader_handle = registry_.create(); 
  registry_.emplace<ShaderComponent>(shader_handle, shader);

  ShaderResource shader_resource { shader_handle };

  shader_map_.insert_or_assign(shader_path, shader_resource);
  PLOGD << "Loaded shader asset: " << shader_path << " successfully";
}

ModelResource ResourceManager::GetModelResource(const std::string& path) {
  assert(model_map_.find(path) != model_map_.cend() && "Unable to find model asset!");
  PLOG_FATAL_IF(model_map_.find(path) == model_map_.cend()) << "Unable to find model asset: " << path;
  return model_map_[path];
}

ShaderResource ResourceManager::GetShaderResource(const std::string& path) {
  assert(shader_map_.find(path) != shader_map_.cend() && "Unable to find shader asset!");
  PLOG_FATAL_IF(shader_map_.find(path) == shader_map_.cend()) << "Unable to find shader asset: " << path;
  return shader_map_[path];
}

MeshComponent& ResourceManager::GetMeshFromHandle(const entt::entity& handle) {
  return registry_.get<MeshComponent>(handle);
}

ShaderComponent& ResourceManager::GetShaderFromHandle(const entt::entity& handle) {
  return registry_.get<ShaderComponent>(handle);
}

TextureComponent* ResourceManager::GetTextureFromMaterialHandle(const entt::entity& handle) {
  if (!registry_.valid(handle)) {
    return nullptr;
  }

  return &registry_.get<TextureComponent>(handle);
}

MaterialComponent& ResourceManager::GetMaterialFromHandle(const entt::entity& handle) {
  return registry_.get<MaterialComponent>(handle);
}

TransformComponent& ResourceManager::GetLocalTransformFromMeshHandle(const entt::entity& handle) { 
  return registry_.get<TransformComponent>(handle);
}

ResourceManager::~ResourceManager() {
  auto mesh_view = registry_.view<MeshComponent>();
  auto texture_view = registry_.view<TextureComponent>();
  auto shader_view = registry_.view<ShaderComponent>();

  for (auto [entity, mesh] : mesh_view.each()) {
    mesh.vertex_array_.reset();
    mesh.vertex_buffer_.reset();
    mesh.index_buffer_.reset();

    PLOGD << "Deleted mesh";
  }
  for (auto [entity, texture] : texture_view.each()) {
    texture.texture_.reset();
    PLOGD << "Deletd texture";
  }
  for (auto [entity, shader] : shader_view.each()) {
    shader.shader_.reset();
    PLOGD << "Deleted shader";
  }
} 