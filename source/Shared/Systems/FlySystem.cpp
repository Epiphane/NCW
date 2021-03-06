// By Thomas Steinke

#include <RGBLogger/Logger.h>

#include "../Event/NamedEvent.h"
#include "FlySystem.h"
#include "BulletPhysicsSystem.h"

namespace CubeWorld
{

void FlySystem::Configure(Engine::EntityManager&, Engine::EventManager&)
{
}

void FlySystem::Update(Engine::EntityManager& entities, Engine::EventManager&, TIMEDELTA)
{
    int isW = mInput->IsKeyDown(GLFW_KEY_W) ? 1 : 0;
    int isA = mInput->IsKeyDown(GLFW_KEY_A) ? 1 : 0;
    int isS = mInput->IsKeyDown(GLFW_KEY_S) ? 1 : 0;
    int isD = mInput->IsKeyDown(GLFW_KEY_D) ? 1 : 0;
    int isQ = mInput->IsKeyDown(GLFW_KEY_Q) ? 1 : 0;
    int isE = mInput->IsKeyDown(GLFW_KEY_E) ? 1 : 0;

    glm::vec3 flyDirection(0);
    if (isW || isA || isS || isD)
    {
        flyDirection = glm::normalize(glm::vec3(isD - isA, 0, isW - isS));
    }

    entities.Each<Engine::Transform, FlySpeed, BulletPhysics::DynamicBody>([&](Engine::Transform& transform, FlySpeed& fly, BulletPhysics::DynamicBody& body) {
        if (fly.director)
        {
            float current = transform.GetYaw();
            float target = fly.director->GetYaw();
            transform.SetYaw((current + target) / 2);
        }

        glm::vec3 dir = float(fly.speed) * glm::normalize(transform.GetFlatDirection());
        glm::vec3 forward = glm::vec3(dir.x, 0, dir.z);
        glm::vec3 right = glm::vec3(-dir.z, 0, dir.x);

        glm::vec3 velocity = forward * flyDirection.z + right * flyDirection.x;

        if (isQ)
        {
            velocity.y += float(fly.speed);
        }
        if (isE)
        {
            velocity.y -= float(fly.speed);
        }

        body.body->setLinearVelocity(btVector3{velocity.x, velocity.y, velocity.z});
        body.body->setActivationState(1);
    });
}

}; // namespace CubeWorld
