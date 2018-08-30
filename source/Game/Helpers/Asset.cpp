// By Thomas Steinke

#pragma once

#include "Asset.h"

namespace CubeWorld
{

namespace Game
{

namespace Asset
{

std::string Animation(const std::string& animation)
{
   return Paths::Join("Assets", "Animations", animation);
}

std::string Model(const std::string& model)
{
   return Paths::Join("Assets", "Models", model);
}

std::string Font(const std::string& fontName)
{
   return Paths::Join("Assets", "Fonts", fontName + ".ttf");
}

};

}; // namespace Game

}; // namespace CubeWorld
