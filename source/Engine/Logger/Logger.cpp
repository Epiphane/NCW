// By Thomas Steinke

#include <algorithm>
#include <stdio.h>

#include "Logger.h"

namespace CubeWorld
{

namespace Logger
{

std::unique_ptr<LogManager> LogManager::sInstance = nullptr;

LogManager::LogManager()
{
   loggers.clear();
}

LogManager::~LogManager()
{
   for (auto logger = loggers.begin(); logger != loggers.end(); ++logger)
   {
      (*logger)->OnDeregister();
   }
   loggers.clear();
}

LogManager* LogManager::Instance()
{
   if (!sInstance)
   {
      sInstance = std::make_unique<LogManager>();
   }

   return sInstance.get();
}

void LogManager::Log(LogLevel level, const char* message)
{
   const char *prefix;
   switch (level)
   {
   case kDebug:
      prefix = "DEBUG | ";
      break;
   case kInfo:
      prefix = "INFO  | ";
      break;
   case kWarning:
      prefix = "WARN  | ";
      break;
   case kError:
      prefix = "ERROR | ";
      break;
   case kAlways:
      prefix = "ALWAYS| ";
      break;
   default:
      prefix = "??????| ";
   }

   for (auto logger = loggers.begin(); logger != loggers.end(); ++logger)
   {
      (*logger)->Log(prefix);
      (*logger)->Log(message);
      (*logger)->Log("\n");
   }
}

void LogManager::Log(const char* message)
{
   for (auto logger = loggers.begin(); logger != loggers.end(); ++logger)
   {
      (*logger)->Log(message);
      (*logger)->Log("\n");
   }
}

void LogManager::RegisterLogger(Logger* logger)
{
   loggers.push_back(logger);
   logger->OnRegister();
}

void LogManager::DeregisterLogger(Logger* logger)
{
   if (loggers.size() == 0)
   {
      return;
   }

   std::vector<Logger*>::iterator it = std::find(loggers.begin(), loggers.end(), logger);
   if (it != loggers.end())
   {
      loggers.erase(it);
      logger->OnDeregister();
   }
}

}; // namespace Input

}; // namespace CubeWorld