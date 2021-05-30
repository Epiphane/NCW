// By Thomas Steinke

#include "Frustum.h"

namespace CubeWorld::Engine::Graphics
{

/// 
/// 
/// 
void Frustum::FromMatrix(const glm::mat4& matrix)
{
    planes[0].x = matrix[0].w - matrix[0].y;
    planes[0].y = matrix[1].w - matrix[1].y;
    planes[0].z = matrix[2].w - matrix[2].y;
    planes[0].w = matrix[3].w - matrix[3].y;

    planes[1].x = matrix[0].w + matrix[0].y;
    planes[1].y = matrix[1].w + matrix[1].y;
    planes[1].z = matrix[2].w + matrix[2].y;
    planes[1].w = matrix[3].w + matrix[3].y;

    planes[2].x = matrix[0].w - matrix[0].x;
    planes[2].y = matrix[1].w - matrix[1].x;
    planes[2].z = matrix[2].w - matrix[2].x;
    planes[2].w = matrix[3].w - matrix[3].x;

    planes[3].x = matrix[0].w + matrix[0].x;
    planes[3].y = matrix[1].w + matrix[1].x;
    planes[3].z = matrix[2].w + matrix[2].x;
    planes[3].w = matrix[3].w + matrix[3].x;

    planes[4].x = matrix[0].w - matrix[0].z;
    planes[4].y = matrix[1].w - matrix[1].z;
    planes[4].z = matrix[2].w - matrix[2].z;
    planes[4].w = matrix[3].w - matrix[3].z;

    planes[5].x = matrix[0].w + matrix[0].z;
    planes[5].y = matrix[1].w + matrix[1].z;
    planes[5].z = matrix[2].w + matrix[2].z;
    planes[5].w = matrix[3].w + matrix[3].z;
}

/// 
/// 
/// 
Frustum Frustum::Create(const glm::mat4& matrix)
{
    Frustum result;
    result.FromMatrix(matrix);
    return result;
}

/// 
/// 
/// 
bool Frustum::Contains(const AABB& box) const
{
    glm::vec3 corners[] = {
        {box.min.x, box.min.y, box.min.z},
        {box.min.x, box.min.y, box.max.z},
        {box.min.x, box.max.y, box.min.z},
        {box.min.x, box.max.y, box.max.z},
        {box.max.x, box.min.y, box.min.z},
        {box.max.x, box.min.y, box.max.z},
        {box.max.x, box.max.y, box.min.z},
        {box.max.x, box.max.y, box.max.z},
    };

    for (size_t p = 0; p < 6; ++p)
    {
        bool isInside = false;
        for (size_t c = 0; c < 8 && !isInside; ++c)
        {
            float dot =
                planes[p].x * corners[c].x+
                planes[p].y * corners[c].y +
                planes[p].z * corners[c].z+
                planes[p].w;

            // Short circuit the loop as soon as we get
            // something inside the plane.
            isInside = (dot >= 0);
        }

        if (!isInside)
        {
            return false;
        }
    }

    // For every plane, there is at least one vertex inside it.
    return true;
}

}; // namespace CubeWorld::Engine::Graphics
