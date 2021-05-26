// By Thomas Steinke

#include <imgui.h>

#include <RGBFileSystem/File.h>
#include <RGBFileSystem/FileSystem.h>
#include <RGBLogger/Logger.h>
#include <RGBNetworking/YAMLSerializer.h>
#include <RGBSettings/SettingsProvider.h>
#include <RGBText/Encoding.h>
#include <Engine/Core/Window.h>
#include <Shared/Helpers/Asset.h>

#include <Shared/Imgui/Extensions.h>
#include "Tools.h"

namespace CubeWorld
{

namespace Editor
{

namespace ShaderLand
{

Tools::Tools(Engine::UIRoot* root, UIElement* parent)
   : UIElement(root, parent, "ShaderLandTools")
{
   mFilename = SettingsProvider::Instance().Get("shader_land", "filename").GetStringValue();

   if (auto[_, exists] = DiskFileSystem{}.Exists(mFilename); !exists)
   {
      mFilename = Asset::Particle("fire");
   }
}

void Tools::Update(TIMEDELTA)
{
}

void Tools::LoadNewFile()
{
   std::string file = OpenFileDialog(mFilename, {});
   if (!file.empty())
   {
      mFilename = file;
      SettingsProvider::Instance().Set("particle_space", "filename", file);
      LoadFile(file);
   }
}

void Tools::LoadFile(const std::string&)
{
}

void Tools::SaveNewFile()
{
   std::string file = SaveFileDialog(mFilename);
   if (!file.empty())
   {
      mFilename = file;
      SaveFile();
   }
}

void Tools::SaveFile()
{
}

}; // namespace ShaderLand

}; // namespace Editor

}; // namespace CubeWorld
