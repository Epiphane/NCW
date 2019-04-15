// By Thomas Steinke

#pragma once

#include <string>
#include <vector>

#include <RGBDesignPatterns/Maybe.h>

namespace CubeWorld
{

namespace Paths
{

bool IsAbsolute(const std::string& path);
bool IsRoot(const std::string& path);

std::string Join(const std::string& part1, const std::string& part2);

template <typename ...Strings>
std::string Join(const std::string& part1, const std::string& part2, Strings&&... parts)
{
   return Join(Join(part1, part2), std::forward<Strings>(parts)...);
}

std::string Normalize(std::string path);
std::string Canonicalize(const std::string& path);

std::string GetFilename(const std::string& path);
std::string GetDirectory(const std::string& path);

std::string GetWorkingDirectory();

}; // namespace Paths

}; // namespace CubeWorld
