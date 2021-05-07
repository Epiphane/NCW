// By Thomas Steinke

#include "../Core/FileSystemProvider.h"
#include "../Event/EventManager.h"
#include "JSScript.h"
#include "JSUtils.h"

namespace CubeWorld
{

namespace Engine
{

namespace JSUtils
{

///
///
///
duk_ret_t Log(const CubeWorld::Logger::LogLevel level, duk_context* ctx)
{
    duk_idx_t nargs;
    const duk_uint8_t* buf;
    duk_size_t sz_buf;
    const char nl = '\0';
    duk_uint8_t buf_stack[256];

    nargs = duk_get_top(ctx);

    /* If argument count is 1 and first argument is a buffer, write the buffer
     * as raw data into the file without a newline; this allows exact control
     * over stdout/stderr without an additional entrypoint (useful for now).
     * Otherwise current print/alert semantics are to ToString() coerce
     * arguments, join them with a single space, and append a newline.
     */

    if (nargs == 1 && duk_is_buffer_data(ctx, 0)) {
        buf = (const duk_uint8_t*)duk_get_buffer_data(ctx, 0, &sz_buf);
    }
    else if (nargs > 0) {
        duk_idx_t i;
        duk_size_t sz_str;
        const duk_uint8_t* p_str;
        duk_uint8_t* p;

        sz_buf = (duk_size_t)nargs;  /* spaces (nargs - 1) + newline */
        for (i = 0; i < nargs; i++) {
            (void)duk_to_lstring(ctx, i, &sz_str);
            sz_buf += sz_str;
        }

        if (sz_buf <= sizeof(buf_stack)) {
            p = (duk_uint8_t*)buf_stack;
        }
        else {
            p = (duk_uint8_t*)duk_push_fixed_buffer(ctx, sz_buf);
        }

        buf = (const duk_uint8_t*)p;
        for (i = 0; i < nargs; i++) {
            p_str = (const duk_uint8_t*)duk_get_lstring(ctx, i, &sz_str);
            memcpy((void*)p, (const void*)p_str, sz_str);
            p += sz_str;
            *p++ = (duk_uint8_t)(i == nargs - 1 ? '\0' : ' ');
        }
    }
    else {
        buf = (const duk_uint8_t*)&nl;
        sz_buf = 1;
    }

    /* 'buf' contains the string to write, 'sz_buf' contains the length
     * (which may be zero).
     */

    std::string message((char*)buf, sz_buf);
    if (sz_buf > 0) {
        LOG(level, message);
    }

    return 0;
}

///
///
///
static duk_ret_t LogMessage(duk_context* ctx)
{
    return Log(CubeWorld::Logger::LogLevel::kAlways, ctx);
}

///
///
///
static duk_ret_t LogError(duk_context* ctx)
{
    return Log(CubeWorld::Logger::LogLevel::kError, ctx);
}

///
///
/// 
duk_ret_t Emit(duk_context* ctx)
{
    JSScript* jsScript;
    {
        DUK_GUARD_SCOPE();

        duk_push_global_object(ctx);
        if (!duk_get_prop_string(ctx, -1, "__script"))
        {
            duk_pop(ctx);
            return DUK_RET_ERROR;
        }

        JSScript* jsScript = static_cast<JSScript*>(duk_get_pointer(ctx, -1));
        duk_pop(ctx);

        if (jsScript == nullptr)
        {
            return DUK_RET_ERROR;
        }
    }

    EventManager* events = jsScript->GetEventManager();
    if (events == nullptr)
    {
        return DUK_RET_ERROR;
    }

    // At least 1 argument required.
    duk_idx_t nargs = duk_get_top(ctx);
    if (nargs == 0) {
        return DUK_RET_TYPE_ERROR;
    }

    const char* name = duk_require_string(ctx, 0);
    BindingProperty data;

    if (nargs > 1)
    {
        return DUK_RET_ERROR;
    }

    events->Emit<JavascriptEvent>(name, std::move(data));
    return 0;
}

///
///
///
void FatalHandler(void* udata, const char* msg)
{
    CUBEWORLD_UNREFERENCED_PARAMETER(udata);
    LOG_ERROR("javascript error: {}", msg);

    if (sCurrentContext != nullptr)
    {
        LOG_INFO("Disabling component");
        sCurrentContext->disabled = true;
    }
}

}; // namespace JSUtils
}; // namespace Engine
}; // namespace CubeWorld
