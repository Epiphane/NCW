////////////////////////////////////////////////////////////////////////////////////////////////////
// NoesisGUI - http://www.noesisengine.com
// Copyright (c) 2013 Noesis Technologies S.L. All Rights Reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////


#include <NsApp/Launcher.h>
#include <NsCore/Init.h>
#include <NsCore/Error.h>
#include <NsCore/Log.h>
#include <NsCore/UTF8.h>
#include <NsCore/StringUtils.h>
#include <NsApp/CommandLine.h>

#include <stdio.h>

#ifdef NS_PLATFORM_WINDOWS
    #define WIN32_LEAN_AND_MEAN 1
    #include <windows.h>
#endif

#ifdef NS_PLATFORM_ANDROID
    #include <android/log.h>
#endif

#ifdef NS_PLATFORM_EMSCRIPTEN
    #include <emscripten.h>
#endif


#ifdef NS_APP_FRAMEWORK
    extern "C" void NsRegisterReflection_NoesisApp();
    extern "C" void NsInitPackages_NoesisApp();
    extern "C" void NsShutdownPackages_NoesisApp();
#endif


using namespace Noesis;
using namespace NoesisApp;


static Launcher* gInstance;
static CommandLine gCommandLine;


////////////////////////////////////////////////////////////////////////////////////////////////////
Launcher::Launcher()
{
    NS_ASSERT(gInstance == 0);
    gInstance = this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Launcher::~Launcher()
{
#ifdef NS_APP_FRAMEWORK
    NsShutdownPackages_NoesisApp();
#endif

    Noesis::Shutdown();

    NS_ASSERT(gInstance != 0);
    gInstance = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
Launcher* Launcher::Current()
{
    NS_ASSERT(gInstance != 0);
    return gInstance;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Launcher::RegisterAppComponents()
{
#ifdef NS_APP_FRAMEWORK
    NsRegisterReflection_NoesisApp();
    NsInitPackages_NoesisApp();
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Launcher::SetArguments(int argc, char** argv)
{
    gCommandLine = CommandLine(argc, argv);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
const CommandLine& Launcher::GetArguments() const
{
    return gCommandLine;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Launcher::Init() const
{
#if NS_MINIMUM_LOG_LEVEL != 0xFFFF
    SetLogHandler([](const char*, uint32_t, uint32_t level, const char* channel, const char* msg)
    {
        // By default only global channel is dumped
        bool filter = !StrIsNullOrEmpty(channel);

        // Enable "Binding" channel by command line
        if (gCommandLine.HasOption("log_binding"))
        {
            if (StrEquals(channel, "Binding"))
            {
                filter = false;
            }
        }

        if (!filter)
        {
            level = level > NS_LOG_LEVEL_ERROR ? NS_LOG_LEVEL_ERROR : level;

    #if defined(NS_PLATFORM_ANDROID)
            const int priority[] = { ANDROID_LOG_VERBOSE, ANDROID_LOG_DEBUG, ANDROID_LOG_INFO,
                ANDROID_LOG_WARN, ANDROID_LOG_ERROR};
            (void)__android_log_print(priority[level], "Noesis", "%s", msg);

    #elif defined(NS_PLATFORM_EMSCRIPTEN)
            const int flags[] = { 0, 0, 0, EM_LOG_WARN, EM_LOG_ERROR };
            const char* prefixes[] = { "T", "D", "I", "W", "E" };
            emscripten_log(EM_LOG_CONSOLE | flags[level], "[NOESIS/%s] %s", prefixes[level], msg);

    #else
            char out[512];
            const char* prefixes[] = { "T", "D", "I", "W", "E" };
            snprintf(out, sizeof(out), "[NOESIS/%s] %s\n", prefixes[level], msg);

        #ifdef NS_PLATFORM_WINDOWS
            #ifdef UNICODE
                uint16_t out_[512];
                UTF8::UTF8To16(out, out_, 512);
                OutputDebugString((LPCWSTR)out_); 
            #else
                OutputDebugString(out); 
            #endif
        #else
            fprintf(stderr, "%s", out);
        #endif
    #endif
        }
    });
#endif

    Noesis::Init(NS_LICENSE_NAME, NS_LICENSE_KEY);
    RegisterAppComponents();
    RegisterComponents();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void Launcher::RegisterComponents() const
{
}
