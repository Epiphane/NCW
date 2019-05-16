// By Thomas Steinke

#include <stdio.h>

#include "StdoutLogger.h"

namespace CubeWorld
{

namespace Logger
{

std::unique_ptr<StdoutLogger> StdoutLogger::sInstance = nullptr;

StdoutLogger::StdoutLogger() 
{
   LogManager::Instance().RegisterLogger(this);
}

StdoutLogger::~StdoutLogger()
{
   LogManager::Instance().DeregisterLogger(this);
}

StdoutLogger* StdoutLogger::Instance()
{
   if (!sInstance)
   {
      sInstance = std::make_unique<StdoutLogger>();
   }

   return sInstance.get();
}

void StdoutLogger::Log(const char* message, Color color)
{
   if (color == Default)
   {
      fprintf(stdout, "%s", message);
   }
   else // TODO
   {
#pragma warning(disable : 4129)
      fprintf(stdout, "\e[1;31m%s\e[m", message);
#pragma warning(default : 4129)
   }
}

}; // namespace Input

}; // namespace CubeWorld
