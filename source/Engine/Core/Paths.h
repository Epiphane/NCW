// By Thomas Steinke

#pragma once

#include <string>
#include <vector>

#include <Engine/Core/Maybe.h>

namespace CubeWorld
{

namespace Paths
{

bool IsAbsolute(const std::string& path);
bool IsRoot(const std::string& path);

std::string Join(const std::string& part1, const std::string& part2);

std::string Normalize(std::string path);
std::string Canonicalize(const std::string& path);

Maybe<void> MakeDirectory(const std::string& path);

std::string GetWorkingDirectory();

}; // namespace Paths

}; // namespace CubeWorld
