// By Thomas Steinke

#include <tuple>
#include <RGBFileSystem/FileSystem.h>

#include "Asset.h"

namespace CubeWorld
{

namespace Asset
{

std::string gAssetRoot = "./Assets";

void SetAssetRootDefault()
{
   DiskFileSystem fs;
   // Account for / (relative to repo)
   if (auto [_1, localExists] = fs.Exists("./Assets"); localExists)
   {
      gAssetRoot = "./Assets";
   }
   // Account for running from /tmp/{config}/{target}
   else if (auto [_2, repoExists] = fs.Exists("../../../Assets"); repoExists)
   {
      gAssetRoot = "../../../Assets";
   }
}

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

std::string Particle(const std::string& particle)
{
   return Paths::Join(gAssetRoot, "Particles", particle, particle + ".yaml");
}

std::string Skeleton(const std::string& skeleton)
{
   return Paths::Join(gAssetRoot, "Skeletons", skeleton);
}

std::string UIElement(const std::string& elementName)
{
   return Paths::Join(gAssetRoot, "UI", elementName);
}

};

}; // namespace CubeWorld
