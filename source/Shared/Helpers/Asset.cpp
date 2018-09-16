// By Thomas Steinke

#include "Asset.h"

namespace CubeWorld
{

namespace Asset
{

std::string gAssetRoot = ".";

void SetAssetRoot(std::string root)
{
   gAssetRoot = Paths::Normalize(root);
}

std::string Animation(const std::string& animation)
{
   return Paths::Join("Assets", "Animations", animation);
}

std::string Model(const std::string& model)
{
   return Paths::Join("Assets", "Models", model);
}

std::string Image(const std::string& model)
{
   return Paths::Join("Assets", "Images", model);
}

std::string Font(const std::string& fontName)
{
   return Paths::Join("Assets", "Fonts", fontName + ".ttf");
}

};

}; // namespace CubeWorld
