// By Thomas Steinke

#pragma once

#include <GL/includes.h>

#include "Bounded.h"
#include "Maybe.h"
#include "Singleton.h"
#include "../Graphics/VAO.h"

namespace CubeWorld
{

std::string OpenFileDialog(
   const std::string& defaultFile,
   const std::vector<std::string>& fileTypes
);

std::string SaveFileDialog(
   const std::string& defaultFile
);

}; // namespace CubeWorld
