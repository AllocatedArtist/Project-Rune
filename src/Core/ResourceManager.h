#ifndef RESOURCE_MANAGER_H_
#define RESOURCE_MANAGER_H_

#include <entt/entt.hpp>

#include "../Components/MeshComponent.h"
#include "../Components/ShaderComponent.h"
#include "../Components/TextureComponent.h"

#include "../Components/MaterialComponent.h"
#include "../Components/TransformComponent.h"

struct ModelResource {
  std::vector<entt::entity> mesh_handles_;
};

struct ShaderResource {
  entt::entity shader_handle_;
};

struct TextureResource {
  entt::entity texture_handle_;
};

class ResourceManager {
public:
  ResourceManager() = default;
  ~ResourceManager(); 

  void LoadModelAsset(const std::string& model_path);
  void LoadShaderAsset(const std::string& shader_path);

  ModelResource GetModelResource(const std::string& path);
  ShaderResource GetShaderResource(const std::string& path);

  MeshComponent& GetMeshFromHandle(const entt::entity& handle);
  MaterialComponent& GetMaterialFromHandle(const entt::entity& handle);
  TransformComponent& GetLocalTransformFromMeshHandle(const entt::entity& handle);
  ShaderComponent& GetShaderFromHandle(const entt::entity& handle);

  TextureComponent* GetTextureFromMaterialHandle(const entt::entity& handle);
private:
  entt::registry registry_;

  std::map<std::string, MaterialComponent> material_map_;
  std::map<std::string, ModelResource> model_map_;
  std::map<std::string, ShaderResource> shader_map_;
};

#endif