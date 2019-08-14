// By Thomas Steinke

#pragma once

#include <string>
#include <vector>

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
