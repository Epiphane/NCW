// By Thomas Steinke

#pragma once

#include <memory>
#include <unordered_map>

#include <Engine/Core/Maybe.h>
#include "../Voxel.h"

namespace CubeWorld
{

namespace Voxel
{

class CubeFormat {
public:
   // Load performs a read, and then buffers that data to the GPU as well.
   static Model* Load(const std::string& path, bool tintable);

   static Maybe<std::unique_ptr<ModelData>> Read(const std::string& path, bool tintable);
   static Maybe<void> Write(const std::string& path, const ModelData& model);

private:
   static std::unordered_map<std::string, std::unique_ptr<Model>> sModels;
};

}; // namespace Voxel

}; // namespace CubeWorld
