// By Thomas Steinke

#include "Asset.h"

namespace CubeWorld
{

namespace Asset
{

std::string gAssetRoot = "./Assets";

void SetAssetRoot(std::string root)
{
   gAssetRoot = Paths::Normalize(root);
}

std::string Animation(const std::string& animation)
{
   return Paths::Join(gAssetRoot, "Animations", animation);
}

std::string Model(const std::string& model)
{
   return Paths::Join(gAssetRoot, "Models", model);
}

std::string Image(const std::string& model)
{
   return Paths::Join(gAssetRoot, "Images", model);
}

std::string Font(const std::string& fontName)
{
   return Paths::Join(gAssetRoot, "Fonts", fontName + ".ttf");
}

};

}; // namespace CubeWorld
