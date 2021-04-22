// By Thomas Steinke

#pragma once

#include <Engine/Core/Input.h>
#include <Engine/System/System.h>

namespace CubeWorld
{

struct Javascript : public Engine::Component<Javascript>
{
    Javascript(const std::string& scriptPath)
        : scriptPath(scriptPath)
    {};
   
    std::string scriptPath;
    void* ctx = nullptr;
};

class JavascriptSystem : public Engine::System<JavascriptSystem>, public Engine::Receiver<JavascriptSystem>
{
public:
    JavascriptSystem(Engine::Input* input) : mInput(input) {}
    ~JavascriptSystem() {}
   
    void Configure(Engine::EntityManager&, Engine::EventManager&) override;
    void Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA dt) override;

    void Receive(const Engine::ComponentAddedEvent<Javascript>& evt);
    void Receive(const Engine::ComponentRemovedEvent<Javascript>& evt);
   
private:
    Engine::Input* mInput;
};

}; // namespace CubeWorld
