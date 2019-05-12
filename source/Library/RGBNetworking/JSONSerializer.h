// By Thomas Steinke

#pragma once

#include <string>

#include <RGBFileSystem/FileSystem.h>
#include <RGBBinding/BindingProperty.h>
#include <RGBDesignPatterns/Maybe.h>

namespace CubeWorld
{

class JSONSerializer
{
public:
   static Maybe<BindingProperty> Deserialize(const std::string& buffer);
   static Maybe<BindingProperty> DeserializeFile(FileSystem& fs, const std::string& path);
   static Maybe<BindingProperty> DeserializeFile(const std::string& path);

   static Maybe<std::string> Serialize(const BindingProperty& data);
   static Maybe<void> SerializeFile(FileSystem& fs, const std::string& path, const BindingProperty& data);
   static Maybe<void> SerializeFile(const std::string& path, const BindingProperty& data);
};

}; // namespace CubeWorld
