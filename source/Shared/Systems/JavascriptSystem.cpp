// By Thomas Steinke

#include <duktape.h>
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <RGBBinding/BindingProperty.h>
#include <RGBFileSystem/FileSystem.h>
#include <RGBNetworking/JSONSerializer.h>
#include <RGBDesignPatterns/Macros.h>
#include <RGBLogger/Logger.h>

#include "../Event/NamedEvent.h"
#include "JavascriptSystem.h"

namespace CubeWorld
{

/// +---------------------------------------------------------------------------------------------+
/// |                                                                                             |
/// |                                Javascript Utilities                                         |
/// |                                                                                             |
/// +---------------------------------------------------------------------------------------------+

///
///
///
static duk_ret_t JavascriptSystem_Log(const CubeWorld::Logger::LogLevel level, duk_context* ctx)
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
static duk_ret_t JavascriptSystem_Print(duk_context* ctx)
{
    return JavascriptSystem_Log(CubeWorld::Logger::LogLevel::kAlways, ctx);
}

///
///
///
static duk_ret_t JavascriptSystem_Alert(duk_context* ctx)
{
    return JavascriptSystem_Log(CubeWorld::Logger::LogLevel::kError, ctx);
}

///
///
/// 
static Engine::EventManager* JavascriptSystem_Events = nullptr;
static duk_ret_t JavascriptSystem_Emit(duk_context* ctx)
{
    if (JavascriptSystem_Events == nullptr)
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

    JavascriptSystem_Events->Emit<JavascriptEvent>(name, std::move(data));
    return 0;
}

///
///
///
Javascript* JavascriptSystem::sCurrentContext = nullptr;
void JavascriptSystem::FatalHandler(void* udata, const char* msg)
{
    CUBEWORLD_UNREFERENCED_PARAMETER(udata);
    LOG_ERROR("javascript error: {}", msg);

    if (sCurrentContext != nullptr)
    {
        LOG_INFO("Disabling component");
        sCurrentContext->disabled = true;
    }
}

/// +---------------------------------------------------------------------------------------------+
/// |                                                                                             |
/// |                                   Javascript Component                                      |
/// |                                                                                             |
/// +---------------------------------------------------------------------------------------------+
Javascript::Javascript(const std::string& scriptPath)
    : scriptPath(scriptPath)
    , _ctx(nullptr)
{
    duk_context* ctx = duk_create_heap(nullptr, nullptr, nullptr, nullptr, JavascriptSystem::FatalHandler);
    if (!ctx) {
        return;
    }

    _ctx = ctx;

    // Push global object onto stack
    duk_push_global_object(ctx);

    // Create a console object with log and error
    {
        // Push "console" and new console object
        duk_push_string(ctx, "console");
        duk_push_object(ctx);

        // Push "log" and log function
        duk_push_string(ctx, "log");
        duk_push_c_function(ctx, JavascriptSystem_Print, DUK_VARARGS);
        duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);

        // Push "error" and error function
        duk_push_string(ctx, "error");
        duk_push_c_function(ctx, JavascriptSystem_Alert, DUK_VARARGS);
        duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);

        // Stack is:
        // 3. console object
        // 2. "console"
        // 1. global object
        duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    }

    {
        // Push "Game" and new object
        duk_push_string(ctx, "Game");
        duk_push_object(ctx);

        // Push "Emit" and Emit function
        duk_push_string(ctx, "Emit");
        duk_push_c_function(ctx, JavascriptSystem_Emit, DUK_VARARGS);
        duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);

        // Put Game in global namespace
        duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
    }

    // Push "component" and this component into the global object.
    duk_push_string(ctx, "component");
    duk_push_pointer(ctx, this);
    duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_CLEAR_WRITABLE | DUK_DEFPROP_CLEAR_CONFIGURABLE);

    // Pop global object
    duk_pop(ctx);

    Reload();
}

Javascript::~Javascript()
{
    if (_ctx)
    {
        duk_destroy_heap((duk_context*)_ctx);
    }
}

void Javascript::Reset()
{
    duk_context* ctx = (duk_context*)_ctx;

    // Reset the global object.
    duk_eval_string(ctx, R"(
        ({
            console: this.console,
            component: this.component,
            Game: this.Game,
        })
    )");
    duk_set_global_object(ctx);
    disabled = false;
    started = false;
}

void Javascript::Reload()
{
    Reset();
    
    // Reload the script.
    DiskFileSystem fs;
    Maybe<std::string> file = fs.ReadEntireFile(scriptPath);
    source = std::move(*file);

    Run();
}

void Javascript::Run()
{
    duk_context* ctx = (duk_context*)_ctx;

    duk_push_string(ctx, scriptPath.c_str());
    duk_compile_string_filename(ctx, 0, source.c_str());
    duk_call(ctx, 0);
    duk_pop(ctx);
}

/// +---------------------------------------------------------------------------------------------+
/// |                                                                                             |
/// |                                   Javascript System                                         |
/// |                                                                                             |
/// +---------------------------------------------------------------------------------------------+
void JavascriptSystem::Configure(Engine::EntityManager&, Engine::EventManager& events)
{
    events.Subscribe<Engine::ComponentAddedEvent<Javascript>>(*this);
    events.Subscribe<Engine::ComponentRemovedEvent<Javascript>>(*this);
}

void JavascriptSystem::Update(Engine::EntityManager& entities, Engine::EventManager& events, TIMEDELTA)
{
    JavascriptSystem_Events = &events;

    entities.Each<Javascript>([&](Engine::Entity entity, Javascript& js) {
        std::string entityName = FormatString("Javascript ({})", entity.GetID().index());
        if (ImGui::Begin(entityName.c_str()))
        {
            if (ImGui::BeginTabBar("Component Properties"))
            {
                if (ImGui::BeginTabItem("Source Code"))
                {
                    ImVec2 size = ImGui::GetContentRegionAvail();
                    size.y -= ImGui::GetItemsLineHeightWithSpacing();
                    ImGui::InputTextMultiline("source", &js.source, size);
                    if (ImGui::Button("Save and run"))
                    {
                        DiskFileSystem fs;
                        if (auto result = fs.WriteFile(js.scriptPath, js.source); !result)
                        {
                            result.Failure().WithContext("Failed saving modified version of script").Log();
                        }

                        js.Reset();
                        js.Run();
                    }
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Properties"))
                {
                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }
        }
        ImGui::End();

        if (js.disabled)
        {
            return;
        }

        sCurrentContext = &js;

        duk_context* ctx = (duk_context*)js._ctx;

        duk_push_global_object(ctx);
        if (!js.started && duk_has_prop_string(ctx, -1, "Start"))
        {
            duk_push_string(ctx, "Start");
            duk_call_prop(ctx, -2, 0);
            duk_pop(ctx);

            js.started = true;
        }

        if (duk_has_prop_string(ctx, -1, "Update"))
        {
            duk_push_string(ctx, "Update");
            duk_call_prop(ctx, -2, 0);
            duk_pop(ctx);
        }
        duk_pop(ctx);

        sCurrentContext = nullptr;
    });

    JavascriptSystem_Events = nullptr;
}

void JavascriptSystem::Receive(const Engine::ComponentAddedEvent<Javascript>&)
{
}

void JavascriptSystem::Receive(const Engine::ComponentRemovedEvent<Javascript>&)
{
}

}; // namespace CubeWorld
