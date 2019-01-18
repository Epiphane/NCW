// By Thomas Steinke

#pragma once

#include <memory>
#include <string>
#include <vector>

#include <Engine/Core/Format.h>

#define NO_ON_REGISTER void OnRegister() override {};
#define NO_ON_DEREGISTER void OnDeregister() override {};
#define NO_REGISTER_HOOKS NO_ON_REGISTER NO_ON_DEREGISTER

namespace CubeWorld
{

namespace Logger
{

class Logger
{
public:
   virtual void OnRegister() {};
   virtual void OnDeregister() {};

public:
   enum Color { Default, Red };

   virtual void Log(const char* message, Color color = Default) = 0;

   template <typename... Args>
   const inline void Log(std::string_view fmt, const Args& ... args) { Log(Format::FormatString(fmt, args...).c_str()); }
   const inline void Log(const std::string& message) { Log(message.c_str()); }
};

enum LogLevel {
   kDebug,
   kInfo,
   kWarning,
   kError,
   kAlways
};

class LogManager : public Logger
{
public:
   NO_REGISTER_HOOKS

public:
   LogManager();
   ~LogManager();

   static LogManager* Instance();

public:
   void Log(const char* message, Color color = Default) override;
   void Log(LogLevel level, const char* message);

   template <typename... Args>
   const inline void Log(LogLevel level, std::string_view fmt, const Args& ... args) { Log(level, Format::FormatString(fmt, args...).c_str()); }
   const inline void Log(LogLevel level, const std::string& message) { Log(level, message.c_str()); }

private:
   static std::unique_ptr<LogManager> sInstance;

private:
   std::vector<Logger*> loggers;

public:
   void RegisterLogger(Logger* logger);
   void DeregisterLogger(Logger* logger);
};

#define LOG_DEBUG(fmt, ...)   (CubeWorld::Logger::LogManager::Instance()->Log(CubeWorld::Logger::LogLevel::kDebug, fmt, ## __VA_ARGS__))
#define LOG_INFO(fmt, ...)    (CubeWorld::Logger::LogManager::Instance()->Log(CubeWorld::Logger::LogLevel::kInfo, fmt, ## __VA_ARGS__))
#define LOG_WARNING(fmt, ...) (CubeWorld::Logger::LogManager::Instance()->Log(CubeWorld::Logger::LogLevel::kWarning, fmt, ## __VA_ARGS__))
#define LOG_ERROR(fmt, ...)   (CubeWorld::Logger::LogManager::Instance()->Log(CubeWorld::Logger::LogLevel::kError, fmt, ## __VA_ARGS__))
#define LOG_ALWAYS(fmt, ...)  (CubeWorld::Logger::LogManager::Instance()->Log(CubeWorld::Logger::LogLevel::kAlways, fmt, ## __VA_ARGS__))

}; // namespace Logger

}; // namespace CubeWorld
