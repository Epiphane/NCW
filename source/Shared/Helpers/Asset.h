// By Thomas Steinke

#pragma once

#include <string>

#include <Engine/Core/Paths.h>

namespace CubeWorld
{

namespace Asset
{

void SetAssetRoot(std::string root);

std::string Animation(const std::string& animation);
std::string Model(const std::string& model);
std::string Image(const std::string& model);
std::string Font(const std::string& fontName);

};

}; // namespace CubeWorld
