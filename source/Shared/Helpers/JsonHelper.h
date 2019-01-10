//
// Created by Elliot Fiske on 11/19/18.
//

#pragma once

#include <glm/glm.hpp>

#include <Engine/Core/Maybe.h>
#include <Shared/Helpers/json.hpp>

namespace CubeWorld
{

namespace Shared
{

glm::vec3      JsonToVec3(const nlohmann::json &json);
nlohmann::json Vec3ToJson(glm::vec3 vec3);

glm::vec4      JsonToVec4(const nlohmann::json &json);
nlohmann::json Vec4ToJson(glm::vec4 vec4);
   
//
// Reads a JSON file and returns the appropriate JSON object.
//
Maybe<nlohmann::json> GetJsonFromFile(const std::string& filename);

} // JsonHelpers

} // Shared
