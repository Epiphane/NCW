// By Thomas Steinke

#pragma once

#include <unordered_map>

#include <RGBBinding/BindingProperty.h>
#include <RGBDesignPatterns/Maybe.h>
#include <RGBDesignPatterns/Singleton.h>

namespace CubeWorld
{

class SettingsProvider : public Singleton<SettingsProvider>
{
public:
   SettingsProvider();
   ~SettingsProvider();

   //
   // Set the local path to settings.
   // Example: "Editor/local"
   //   Win: C:/Users/<user>/AppData/Local/NCW/Editor/local
   //   Mac: /Users/Shared/NCW/Editor/local
   //
   void SetLocalPath(const std::string& path);

   //
   // Get the directory path where all settings are stored.
   //
   std::string GetSettingsPath() const;

   //
   // Get a setting.
   //
   const BindingProperty& Get(const std::string& scope, const std::string& path) const;

   //
   // Persist a setting.
   //
   void Set(const std::string& scope, const std::string& path, const BindingProperty& property);
   void Set(const std::string& scope, const std::string& path, BindingProperty&& property);

   //
   // Apply a group of settings.
   // This is similar to Set, except that it will not erase settings
   // unless explicitly colliding.
   //
   void Apply(const std::string& scope, const std::string& path, const BindingProperty& property);
   void Apply(const std::string& scope, const std::string& path, BindingProperty&& property);

private:
   void Reload();
   void Persist() const;

private:
   std::string mDataPath;
   std::string mLocalPath;

   BindingProperty mSettings;
};

}; // namespace CubeWorld
