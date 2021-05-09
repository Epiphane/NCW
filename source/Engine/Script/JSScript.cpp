// By Thomas Steinke

#include "../Core/FileSystemProvider.h"
#include "JSScript.h"
#include "JSUtils.h"

namespace CubeWorld
{

namespace Engine
{

/// 
/// 
/// 
JSScript::JSScript()
{
    ctx = duk_create_heap(nullptr, nullptr, nullptr, nullptr, JSUtils::FatalHandler);
    if (ctx == nullptr)
    {
        LOG_ERROR("Unexpected error while initializing duktape heap");
        return;
    }

    DUK_GUARD_SCOPE();

    // Push global object onto stack
    duk_push_global_object(ctx);

    //
    // Create a couple objects in the global namespace:
    // 
    // console: Debugging options for scripts
    //  - log: log message
    //  - error: log an error
    // 
    // Game: Options for interacting with the rest of the game
    //  - Emit: Send a message outwards
    //
    // __script: Pointer to this JSScript (used internally)
    //

    // Create a console object with log and error
    {
        DUK_GUARD_SCOPE();

        // Push "console" and new console object
        duk_push_string(ctx, "console");
        duk_push_object(ctx);

        // Push "log" and log function
        duk_push_string(ctx, "log");
        duk_push_c_function(ctx, JSUtils::LogMessage, DUK_VARARGS);
        duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);

        // Push "error" and error function
        duk_push_string(ctx, "error");
        duk_push_c_function(ctx, JSUtils::LogError, DUK_VARARGS);
        duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);

        // Stack is:
        // 3. console object
        // 2. "console"
        // 1. global object
        duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    }

    {
        DUK_GUARD_SCOPE();

        // Push "Game" and new object
        duk_push_string(ctx, "Game");
        duk_push_object(ctx);

        // Push "Emit" and Emit function
        duk_push_string(ctx, "Emit");
        duk_push_c_function(ctx, JSUtils::Emit, DUK_VARARGS);
        duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);

        // Put Game in global namespace
        duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    }

    // Push "__script" and new object
    duk_push_string(ctx, "__script");
    duk_push_pointer(ctx, this);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_CLEAR_WRITABLE | DUK_DEFPROP_CLEAR_CONFIGURABLE);

    // Push "__userdata" and new object
    duk_push_string(ctx, "__userdata");
    duk_push_object(ctx);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_CLEAR_WRITABLE | DUK_DEFPROP_CLEAR_CONFIGURABLE);

    // Pop global object
    duk_pop(ctx);
}

/// 
/// 
/// 
JSScript::~JSScript()
{
    if (ctx != nullptr)
    {
        duk_destroy_heap(ctx);
    }
}

/// 
/// 
/// 
void JSScript::LoadSource(const std::string& source, const std::optional<std::string>& name)
{
    DUK_GUARD_SCOPE();

    if (name)
    {
        duk_push_string(ctx, name->c_str());
        duk_compile_string_filename(ctx, 0, source.c_str());
    }
    else
    {
        duk_compile_string(ctx, 0, source.c_str());
    }
    duk_call(ctx, 0);
    duk_pop(ctx);
}

/// 
/// 
/// 
void JSScript::LoadFile(const std::string& path)
{
    FileSystem& fs = FileSystemProvider::Instance();

    Maybe<std::string> source = fs.ReadEntireFile(path);
    if (!source)
    {
        source.Failure().WithContext("Failed reading script file").Log();
        return;
    }

    LoadSource(*source);
}

void JSScript::Reset()
{
    DUK_GUARD_SCOPE();

    // Reset the global object.
    duk_eval_string(ctx, R"(
        ({
            console: this.console,
            Game: this.Game,
            __script: this.__script,
            __userdata: this.__userdata,
        })
    )");
    duk_set_global_object(ctx);

    SetNotFailed();
}

void JSScript::RunFunction(const std::string& fnName)
{
    DUK_GUARD_SCOPE();

    duk_push_global_object(ctx);
    if (duk_has_prop_string(ctx, -1, fnName.c_str()))
    {
        duk_push_string(ctx, fnName.c_str());
        duk_call_prop(ctx, -2, 0);
        duk_pop(ctx);
    }
    duk_pop(ctx);
}

}; // namespace Engine

}; // namespace CubeWorld
