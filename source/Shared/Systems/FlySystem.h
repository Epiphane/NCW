// By Thomas Steinke

#pragma once

#include <Engine/Core/Input.h>
#include <Engine/System/System.h>

namespace CubeWorld
{

struct FlySpeed : public Engine::Component<FlySpeed>
{
    FlySpeed(Engine::ComponentHandle<Engine::Transform> director, double speed)
        : director(director)
        , speed(speed)
    {};
    FlySpeed(double speed) : speed(speed) {};
   
    Engine::ComponentHandle<Engine::Transform> director;
    double speed;
};

class FlySystem : public Engine::System<FlySystem>
{
public:
    FlySystem(Engine::Input* input) : mInput(input) {}
    ~FlySystem() {}
   
    void Configure(Engine::EntityManager&, Engine::EventManager&) override;
    void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt) override;
   
private:
    Engine::Input* mInput;
};

}; // namespace CubeWorld
