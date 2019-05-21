// By Thomas Steinke

#if CUBEWORLD_PLATFORM_WINDOWS
#include <initguid.h>
#include <Shlobj.h>
#endif

#include <RGBFileSystem/Paths.h>
#include <RGBLogger/Logger.h>
#include <RGBNetworking/YAMLSerializer.h>
#include <RGBText/Encoding.h>
#include <RGBText/StringHelper.h>

#include "SettingsProvider.h"

namespace CubeWorld
{

SettingsProvider::SettingsProvider()
   : mDataPath("")
   , mLocalPath("")
   , mSettings(BindingProperty::Type::kObjectType)
{
#if CUBEWORLD_PLATFORM_WINDOWS
   PWSTR path = nullptr;
   HRESULT hr = SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &path);

   if (!SUCCEEDED(hr))
   {
      LOG_ERROR("Failed getting local data path: Error %1", hr);
      return;
   }

   mDataPath = Paths::Join(WideToUtf8(path), "NCW");
   CoTaskMemFree(path);
#elif CUBEWORLD_PLATFORM_MACOSX
   mDataPath = "/Users/Shared/NCW";
#endif

   Reload();
}

SettingsProvider::~SettingsProvider()
{
   Persist();
}

void SettingsProvider::SetLocalPath(const std::string& path)
{
   mLocalPath = path;

   Reload();
}

std::string SettingsProvider::GetSettingsPath() const
{
   return Paths::Join(mDataPath, mLocalPath);
}

const BindingProperty& SettingsProvider::Get(const std::string& scope, const std::string& path) const
{
   std::vector<std::string> parts = StringHelper::Split(path, '.');

   const BindingProperty* cursor = &mSettings[scope];

   for (const std::string& part : parts)
   {
      cursor = &(*cursor)[part];
   }

   return *cursor;
}

void SettingsProvider::Set(const std::string& scope, const std::string& path, const BindingProperty& property)
{
   std::vector<std::string> parts = StringHelper::Split(path, '.');

   BindingProperty* cursor = &mSettings[scope];

   for (const std::string& part : parts)
   {
      cursor = &(*cursor)[part];
   }

   *cursor = property;

   Persist();
}

void SettingsProvider::Set(const std::string& scope, const std::string& path, BindingProperty&& property)
{
   Set(scope, path, property);
}

void SettingsProvider::Apply(const std::string& scope, const std::string& path, const BindingProperty& property)
{
   if (!property.IsObject())
   {
      Set(scope, path, property);
      return;
   }

   std::vector<std::string> parts = StringHelper::Split(path, '.');

   BindingProperty* cursor = &mSettings[scope];

   for (const std::string& part : parts)
   {
      cursor = &(*cursor)[part];
   }

   for (const auto& [key, val] : property.pairs())
   {
      (*cursor)[key] = val;
   }

   Persist();
}

void SettingsProvider::Apply(const std::string& scope, const std::string& path, BindingProperty&& property)
{
   if (!property.IsObject())
   {
      Set(scope, path, std::move(property));
      return;
   }

   Apply(scope, path, property);
}

void SettingsProvider::Reload()
{
   if (mDataPath.empty())
   {
      return;
   }

   DiskFileSystem fs;
   std::string path = Paths::Join(mDataPath, mLocalPath, "settings.yaml");
   if (auto [_, exists] = fs.Exists(path); exists)
   {
      Maybe<BindingProperty> settings = YAMLSerializer::DeserializeFile(path);
      if (!settings)
      {
         LOG_ERROR("Failed loading settings from disk: %1", settings.Failure().GetMessage());
      }
      else
      {
         mSettings = std::move(*settings);
      }
   }
}

void SettingsProvider::Persist() const
{
   if (mDataPath.empty())
   {
      return;
   }

   std::string basePath = Paths::Join(mDataPath, mLocalPath);

   Maybe<void> makeFolders = DiskFileSystem{}.MakeDirectory(basePath);
   if (!makeFolders)
   {
      LOG_ERROR("Failed creating location to store settings: %1", makeFolders.Failure().GetMessage());
   }

   Maybe<void> writeResult = YAMLSerializer::SerializeFile(Paths::Join(basePath, "settings.yaml"), mSettings);
   if (!writeResult)
   {
      LOG_ERROR("Failed writing settings to disk: %1", writeResult.Failure().GetMessage());
   }
}

}; // namespace CubeWorld
