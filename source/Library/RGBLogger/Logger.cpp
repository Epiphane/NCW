// By Thomas Steinke

#include <algorithm>

#include "Logger.h"

namespace CubeWorld
{

namespace Logger
{

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

void LogManager::Log(const char* message, Color color)
{
   for (auto logger = loggers.begin(); logger != loggers.end(); ++logger)
   {
      (*logger)->Log(message, color);
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