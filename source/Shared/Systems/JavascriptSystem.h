// By Thomas Steinke

#pragma once

#include <Engine/Core/Input.h>
#include <Engine/Event/Event.h>
#include <Engine/System/System.h>
#include <RGBBinding/BindingProperty.h>

namespace CubeWorld
{

struct JavascriptEvent : public Engine::Event<JavascriptEvent>
{
    JavascriptEvent(const std::string& name) : name(name) {}
    JavascriptEvent(const std::string& name, BindingProperty&& data)
        : name(name)
        , data(std::move(data))
    {}

    std::string name;
    BindingProperty data;
};

struct Javascript : public Engine::Component<Javascript>
{
    Javascript(const std::string& scriptPath);
    ~Javascript();
    void Reset();
    void Reload();
    void Run();
   
    std::string scriptPath;
    void* _ctx = nullptr;
    bool disabled = false;
    bool started = false;

    std::string source;
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

private:
    friend struct Javascript;
    static void FatalHandler(void* udata, const char* msg);
    static Javascript* sCurrentContext;
};

}; // namespace CubeWorld
