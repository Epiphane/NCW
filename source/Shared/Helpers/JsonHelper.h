//
// Created by Elliot Fiske on 11/19/18.
//

#pragma once

#include <glm/glm.hpp>

#include <Shared/Helpers/json.hpp>

namespace Shared
{

namespace JsonHelpers
{

glm::vec3      JsonToVec3(const nlohmann::json &json);
nlohmann::json Vec3ToJson(glm::vec3 vec3);

glm::vec4      JsonToVec4(const nlohmann::json &json);
nlohmann::json Vec4ToJson(glm::vec4 vec4);

} // JsonHelpers

} // Shared
