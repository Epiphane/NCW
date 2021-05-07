// By Thomas Steinke

#pragma once

#include <string>
#include <duktape.h>
#include <RGBBinding/BindingProperty.h>

#include "../Event/Event.h"

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

namespace Engine
{

class EventManager;

class JSScript
{
public:
    JSScript();
    ~JSScript();

    void LoadSource(const std::string& source, const std::optional<std::string>& name = std::nullopt);
    void LoadFile(const std::string& path);

    void RunFunction(const std::string& fnName);
    void Reset();

    void SetNotFailed() { failed = false; }
    bool HasFailure() const { return failed; }

    EventManager* GetEventManager() const { return mEventManager; }
    void SetEventManager(EventManager* events) { mEventManager = events; }

private:
    duk_context* ctx = nullptr;
    bool failed = false;

    EventManager* mEventManager = nullptr;
};

}; // namespace Engine

}; // namespace CubeWorld
