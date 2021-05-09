// By Thomas Steinke

#pragma once

#include <string>
#include <duktape.h>
#include <RGBBinding/BindingProperty.h>

#include "JSUtils.h"
#include "../Event/Event.h"

struct ImGuiInputTextCallbackData;

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
    void Reset();

    void PushVariable(int number)
    {
        duk_push_number(ctx, number);
    }

    template<typename T, typename ...Args>
    void PushVariable(const T& var, const Args&... args)
    {
        PushVariable(var);
        PushVariable(args...);
    }

    void RunFunction(const std::string& fnName);

    template<typename ...Args>
    void RunFunction(const std::string& fnName, const Args&... args)
    {
        DUK_GUARD_SCOPE();
        duk_idx_t global = duk_get_top(ctx);
        duk_push_global_object(ctx);
        if (duk_has_prop_string(ctx, -1, fnName.c_str()))
        {
            const size_t nargs = sizeof...(Args);
            duk_push_string(ctx, fnName.c_str());
            PushVariable(args...);
            duk_call_prop(ctx, global, nargs);
            duk_pop(ctx);
        }
        duk_pop(ctx);
    }

    duk_context* GetContext() const { return ctx; }

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
