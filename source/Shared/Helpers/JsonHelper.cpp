//
// Created by Elliot Fiske on 11/19/18.
//

#include "JsonHelper.h"


namespace Shared
{

namespace JsonHelpers
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

} // JsonHelpers

} // Shared
