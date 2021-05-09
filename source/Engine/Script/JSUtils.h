// By Thomas Steinke

#pragma once

#include <RGBLogger/Logger.h>
#include <duktape.h>

namespace CubeWorld
{

namespace Engine
{

namespace JSUtils
{

//
//
//  Uti
// 
//

struct JSScopeGuard
{
    JSScopeGuard(duk_context* ctx)
        : ctx(ctx)
        , top(duk_get_top(ctx))
    {}
    ~JSScopeGuard()
    {
        assert(duk_get_top(ctx) == top && "duk_context changed the size of the stack and exited");
    }

    duk_context* ctx;
    duk_idx_t top;
};

#define _DUK_GUARD_SCOPE_IN(name, line) ::CubeWorld::Engine::JSUtils::JSScopeGuard name##line(ctx);
#define _DUK_GUARD_SCOPE(line) _DUK_GUARD_SCOPE_IN(_scope_, line)
#define DUK_GUARD_SCOPE() _DUK_GUARD_SCOPE(__LINE__)

duk_ret_t Log(const CubeWorld::Logger::LogLevel level, duk_context* ctx);
duk_ret_t LogMessage(duk_context* ctx);
duk_ret_t LogError(duk_context* ctx);
duk_ret_t Emit(duk_context* ctx);
void FatalHandler(void* udata, const char* msg);

}; // namespace JSUtils
}; // namespace Engine
}; // namespace CubeWorld
