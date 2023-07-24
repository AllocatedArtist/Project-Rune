#ifndef MODEL_COMPONENT_H_
#define MODEL_COMPONENT_H_

#include <vector>

#include "../Graphics/ModelLoader.h"

struct ModelComponent {
  std::vector<Mesh> meshes_;

  ModelComponent() = default;
  ModelComponent(const std::string& filename) {
    Model model;
    model.LoadModel(filename);
    meshes_ = model.GetMeshes();
  }
};


#endif