// By Thomas Steinke

#pragma once

#include <string>

#include <RGBFileSystem/Paths.h>

namespace CubeWorld
{

namespace Asset
{

void SetAssetRoot(std::string root);

std::string Animation(const std::string& animation);
std::string Model(const std::string& model);
std::string Image(const std::string& model);
std::string Font(const std::string& fontName);
std::string UIElement(const std::string& elementName);

};

}; // namespace CubeWorld
