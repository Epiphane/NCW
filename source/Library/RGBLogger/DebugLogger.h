// By Thomas Steinke

#pragma once

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
   virtual ~DebugLogger();

   void OnRegister() override;
   void OnDeregister() override;

   static DebugLogger* Instance();

public:
   void Log(const char* message, Color color = Color::Default) override;

private:
   static std::unique_ptr<DebugLogger> sInstance;
};

}; // namespace Logger

}; // namespace CubeWorld
