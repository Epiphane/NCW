// By Thomas Steinke

#pragma once

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

#include <RGBBinding/BindingPropertyReader.h>
#include "JSONSerializer.h"

namespace CubeWorld
{

Maybe<BindingProperty> JSONSerializer::Deserialize(const std::string& buffer)
{
   return BindingPropertyReader{}.Read(buffer);
}

Maybe<BindingProperty> JSONSerializer::DeserializeFile(FileSystem& fs, const std::string& path)
{
   Maybe<std::string> maybeResult = fs.ReadEntireFile(path);
   if (!maybeResult)
   {
      return maybeResult.Failure().WithContext("Failed reading file");
   }

   return Deserialize(std::move(*maybeResult));
}

Maybe<BindingProperty> JSONSerializer::DeserializeFile(const std::string& path)
{
   DiskFileSystem fs;
   return DeserializeFile(fs, path);
}

Maybe<std::string> JSONSerializer::Serialize(const BindingProperty& data)
{
   rapidjson::StringBuffer buffer;
   rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

   Maybe<void> result = data.Write(writer);
   if (!result)
   {
      return result.Failure();
   }
   return std::string{buffer.GetString(), buffer.GetSize()};
}

Maybe<void> JSONSerializer::SerializeFile(FileSystem& fs, const std::string& path, const BindingProperty& data)
{
   Maybe<std::string> serialized = Serialize(data);
   if (!serialized)
   {
      return serialized.Failure().WithContext("Failed to serialize data");
   }
   
   return fs.WriteFile(path, std::move(*serialized));
}

Maybe<void> JSONSerializer::SerializeFile(const std::string& path, const BindingProperty& data)
{
   DiskFileSystem fs;
   return SerializeFile(fs, path, data);
}

}; // namespace CubeWorld
