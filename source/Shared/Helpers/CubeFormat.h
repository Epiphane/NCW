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

//
// NOTE: This is deprecated. The cub extension is no longer used by anything, but in the
// interest of allowing for data conversion one day in the future (using MagicaVoxel to
// make mods for real CubeWorld, exporting assets from mods/game to an editable format)
// the read/write functionality is still supported.
//
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
