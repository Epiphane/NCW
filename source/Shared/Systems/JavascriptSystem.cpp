// By Thomas Steinke

#include <RGBBinding/BindingProperty.h>
#include <RGBFileSystem/FileSystem.h>
#include <RGBNetworking/JSONSerializer.h>
#include <RGBDesignPatterns/Macros.h>
#include <RGBLogger/Logger.h>
#include <duktape.h>

#include "../Event/NamedEvent.h"
#include "JavascriptSystem.h"

namespace CubeWorld
{

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

static duk_ret_t JavascriptSystem_Print(duk_context* ctx)
{
    return JavascriptSystem_Log(CubeWorld::Logger::LogLevel::kAlways, ctx);
}

static duk_ret_t JavascriptSystem_Alert(duk_context* ctx)
{
    return JavascriptSystem_Log(CubeWorld::Logger::LogLevel::kError, ctx);
}

static void JavascriptSystem_FatalHandler(void* udata, const char* msg)
{
    CUBEWORLD_UNREFERENCED_PARAMETER(udata);
    LOG_ERROR("javascript error: {}", msg);
}

void JavascriptSystem::Configure(Engine::EntityManager&, Engine::EventManager& events)
{
	events.Subscribe<Engine::ComponentAddedEvent<Javascript>>(*this);
	events.Subscribe<Engine::ComponentRemovedEvent<Javascript>>(*this);
}

void JavascriptSystem::Update(Engine::EntityManager& entities, Engine::EventManager&, TIMEDELTA)
{
    entities.Each<Javascript>([&](Javascript&) {
    });
}

void JavascriptSystem::Receive(const Engine::ComponentAddedEvent<Javascript>& evt)
{
	duk_context* ctx = duk_create_heap(nullptr, nullptr, nullptr, nullptr, JavascriptSystem_FatalHandler);
	if (!ctx) {
		return;
	}

	evt.component->ctx = ctx;
	duk_push_global_object(ctx);
	duk_push_string(ctx, "print");
	duk_push_c_function(ctx, JavascriptSystem_Print, DUK_VARARGS);
	duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
	duk_push_string(ctx, "alert");
	duk_push_c_function(ctx, JavascriptSystem_Alert, DUK_VARARGS);
	duk_def_prop(ctx, -3, DUK_DEFPROP_HAVE_VALUE | DUK_DEFPROP_SET_WRITABLE | DUK_DEFPROP_SET_CONFIGURABLE);
	duk_pop(ctx);

	DiskFileSystem fs;
	Maybe<std::string> file = fs.ReadEntireFile("Scripts/generator.js");
	duk_push_string(ctx, "Scripts/generator.js");
	duk_compile_string_filename(ctx, 0, file->c_str());
	duk_call(ctx, 0);
	LOG_ALWAYS("program result: {}", (double)duk_get_number(ctx, -1));
	duk_pop(ctx);
}

void JavascriptSystem::Receive(const Engine::ComponentRemovedEvent<Javascript>& evt)
{
	if (evt.component->ctx)
	{
		duk_destroy_heap((duk_context *)evt.component->ctx);
	}
}

}; // namespace CubeWorld
