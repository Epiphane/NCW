// By Thomas Steinke

#if CUBEWORLD_PLATFORM_WINDOWS
#include <Windows.h>
#endif

#include "DebugLogger.h"

namespace CubeWorld
{

namespace Logger
{

std::unique_ptr<DebugLogger> DebugLogger::sInstance = nullptr;

DebugLogger::DebugLogger()
{
   LogManager::Instance().RegisterLogger(this);
}

DebugLogger::~DebugLogger()
{
   LogManager::Instance().DeregisterLogger(this);
}

void DebugLogger::OnRegister()
{
   Log("\n\n------------------------- Program started. -------------------------\n\n");
}

void DebugLogger::OnDeregister()
{
   Log("\n\n-------------------------- Program ended. --------------------------\n\n");
}

DebugLogger* DebugLogger::Instance()
{
   if (!sInstance)
   {
      sInstance = std::make_unique<DebugLogger>();
   }

   return sInstance.get();
}

void DebugLogger::Log(const char* message, Color)
{
#if CUBEWORLD_PLATFORM_WINDOWS
   OutputDebugString(message);
#endif
}

}; // namespace Input

}; // namespace CubeWorld
