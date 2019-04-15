// By Thomas Steinke

#pragma once

#include <string>
#include <memory>

#include "Logger.h"

namespace CubeWorld
{

namespace Logger
{

class DebugLogger : public Logger
{
public:
   DebugLogger();
   ~DebugLogger();

   void OnRegister() override;
   void OnDeregister() override;

   static DebugLogger* Instance();

public:
   void Log(const char* message, Color color = Default) override;

private:
   static std::unique_ptr<DebugLogger> sInstance;
};

}; // namespace Logger

}; // namespace CubeWorld
