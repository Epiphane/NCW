// By Thomas Steinke

#include <GL/includes.h>
#include <glm/ext.hpp>
#include <imgui.h>

#include <Engine/Entity/EntityManager.h>

#include "ArmCamera.h"

namespace CubeWorld
{

ArmCamera::ArmCamera(const Engine::ComponentHandle<Engine::Transform>& transform, const Options& options)
    : transform(transform)
    , aspect(options.aspect)
    , fov(options.fov)
    , near(options.near)
    , far(options.far)
    , distance(options.distance)
    , minDistance(options.minDistance)
    , maxDistance(options.maxDistance)
{}

ArmCamera::ArmCamera(
    const Engine::ComponentHandle<Engine::Transform>& transform,
    const BindingProperty& data,
    float aspect
)
    : transform(transform)
    , aspect(aspect)
{
    Binding::deserialize(*this, data);
}

MouseControlledCameraArm::MouseControlledCameraArm(const BindingProperty& data)
{
    Binding::deserialize(*this, data);
}

glm::mat4 ArmCamera::GetPerspective() const
{
    return glm::perspective(fov, aspect, near, far);
}

glm::mat4 ArmCamera::GetView() const
{
    glm::vec3 pos = transform->GetAbsolutePosition();
    glm::vec3 dir = transform->GetAbsoluteDirection();
    return glm::lookAt(pos - dir * distance, pos, glm::vec3(0, 1, 0));
}

glm::vec3 ArmCamera::GetPosition() const
{
    return transform->GetAbsolutePosition();
}

}; // namespace CubeWorld
