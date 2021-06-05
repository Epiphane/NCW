// By Thomas Steinke

#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

#include <RGBBinding/BindingProperty.h>
#include <Engine/Entity/Component.h>
#include <Engine/Graphics/VBO.h>
#include "../Voxel.h"
#include "../Helpers/VoxFormat.h"

namespace CubeWorld
{

//
// Name might have to change someday. Acts as a component, and otherwise, is just a front for 
// Voxel::VoxModel.
//
class VoxModel : public Engine::Component<VoxModel> {
public:
   using Part = Voxel::VoxModel::Part;

public:
   //
   // Creation functions.
   //
   VoxModel();
   VoxModel(const BindingProperty& data);
   VoxModel(const std::string& path, glm::vec3 tint = glm::vec3(255));

   void Load(const std::string& path);
   void Set(Voxel::VoxModel* model);

public:
   // Member data
   glm::vec3 mTint;
   
   std::vector<Part> mParts;
   std::unordered_map<std::string, size_t> mPartLookup;

   Engine::Graphics::VBO mVBO;
};

//class VoxModelComponent : public Engine::Component<VoxModelComponent>, public VoxModel {
//public:
//   VoxModelComponent(const std::string& path, glm::vec3 tint = glm::vec3(255))
//      : VoxModel(path, tint) {};
//};

}; // namespace CubeWorld
