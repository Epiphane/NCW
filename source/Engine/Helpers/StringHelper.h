// By Thomas Steinke

#pragma once

#include <algorithm>
#include <string>
#include <vector>

namespace CubeWorld
{

namespace StringHelper
{

bool EndsWith(const std::string& source, const std::string pattern);
bool StartsWith(const std::string& source, const std::string pattern);
   
std::vector<std::string> Split(const std::string& path, char delim = '/');
std::string Join(const std::vector<std::string>& parts, std::string delim = "/");

}; // namespace StringHelper

}; // namespace CubeWorld