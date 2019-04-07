//
// Created by Elliot Fiske on 11/19/18.
//

#include "JsonHelper.h"

#include <fstream>
#include <sstream>

namespace CubeWorld
{

namespace Shared
{

glm::vec3 JsonToVec3(const nlohmann::json& json)
{
   return glm::vec3(json[0], json[1], json[2]);
}

nlohmann::json Vec3ToJson(glm::vec3 vec3)
{
   return {
         (double)std::round(vec3.x * 100) / 100,
         (double)std::round(vec3.y * 100) / 100,
         (double)std::round(vec3.z * 100) / 100
   };
}

glm::vec4 JsonToVec4(const nlohmann::json& json)
{
   return glm::vec4(json[0], json[1], json[2], json[3]);
}

nlohmann::json Vec4ToJson(glm::vec4 vec4)
{
   return {
         (double)std::round(vec4.x * 100) / 100,
         (double)std::round(vec4.y * 100) / 100,
         (double)std::round(vec4.z * 100) / 100,
         (double)std::round(vec4.w * 100) / 100
   };
}
   
Maybe<nlohmann::json> GetJsonFromFile(const std::string& filename)
{
   nlohmann::json result;
   
   std::ifstream file(filename);
   
   if (!file.good()) {
      return Failure{FAILURE_CODE_JSON_FILE_NOT_FOUND, "Could not open file %1", filename.c_str()};
   }
   
   try {
      file >> result;
   }
   catch(nlohmann::detail::exception e) {
      return Failure{"Parse error in %1: %2", filename.c_str(), e.what()};
   }
   
   return result;
}

} // JsonHelpers

} // Shared
