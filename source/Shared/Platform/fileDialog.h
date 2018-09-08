// By Thomas Steinke

#pragma once

#include <string>
#include <vector>

std::string openFileDialog(
   const std::string& defaultFilename,
   const std::vector<std::string>& fileTypes
);