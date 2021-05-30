// By Thomas Steinke

#pragma once

#include <glm/glm.hpp>
#include "AABB.h"

namespace CubeWorld::Engine::Graphics
{

struct Frustum
{
    glm::vec4 planes[6];

    static Frustum Create(const glm::mat4& matrix);
    void FromMatrix(const glm::mat4& matrix);

    bool Contains(const AABB& box) const;
};

}; // namespace CubeWorld::Engine::Graphics
