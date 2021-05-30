// By Thomas Steinke

#include "Camera.h"

namespace CubeWorld::Engine::Graphics
{

Frustum Camera::GetFrustum() const
{
    return Frustum::Create(GetPerspective() * GetView());
}
   
}; // namespace CubeWorld::Engine::Graphics
