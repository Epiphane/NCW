// By Thomas Steinke

#pragma once

#include <string>
#include <memory>

#include "Logger.h"

namespace CubeWorld
{

namespace Logger
{


class StdoutLogger : public Logger
{
public:
   StdoutLogger();
   virtual ~StdoutLogger();

   NO_REGISTER_HOOKS

   static StdoutLogger* Instance();

public:
   void Log(const char* message, Color color = Color::Default) override;

private:
   static std::unique_ptr<StdoutLogger> sInstance;
};

}; // namespace Logger

}; // namespace CubeWorld
