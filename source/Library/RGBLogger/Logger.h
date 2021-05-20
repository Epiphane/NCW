// By Thomas Steinke

#pragma once

#include <string>
#include <vector>
#include <mutex>

#include <RGBDesignPatterns/Singleton.h>
#include <RGBText/Format.h>

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
   enum Color { Default, Red };

public:
   virtual ~Logger() {}

   virtual void OnRegister() {};
   virtual void OnDeregister() {};
   virtual void Log(const char* message, Color color = Default) = 0;

   template <typename... Args>
   const inline void Log(std::string_view fmt, const Args& ... args) { Log(FormatString(fmt, args...).c_str()); }
   const inline void Log(const std::string& message) { Log(message.c_str()); }
};

enum class LogLevel
{
   kDebug,
   kInfo,
   kWarning,
   kError,
   kAlways
};

class LogManager : public Singleton<LogManager>, public Logger
{
public:
   NO_REGISTER_HOOKS

public:
   LogManager();
   virtual ~LogManager();

public:
   void Log(const char* message, Color color = Default) override;
   void Log(LogLevel level, const char* message);

   template <typename... Args>
   const inline void Log(LogLevel level, std::string_view fmt, const Args& ... args) { Log(level, FormatString(fmt, args...).c_str()); }
   const inline void Log(LogLevel level, const std::string& message) { Log(level, message.c_str()); }

private:
    std::mutex mLogMutex;
    std::vector<Logger*> loggers;

public:
   void RegisterLogger(Logger* logger);
   void DeregisterLogger(Logger* logger);
};

#define LOG(level, fmt, ...)  (CubeWorld::Logger::LogManager::Instance().Log(level, fmt, ## __VA_ARGS__))
#define LOG_DEBUG(fmt, ...)   (LOG(CubeWorld::Logger::LogLevel::kDebug, fmt, ## __VA_ARGS__))
#define LOG_INFO(fmt, ...)    (LOG(CubeWorld::Logger::LogLevel::kInfo, fmt, ## __VA_ARGS__))
#define LOG_WARNING(fmt, ...) (LOG(CubeWorld::Logger::LogLevel::kWarning, fmt, ## __VA_ARGS__))
#define LOG_ERROR(fmt, ...)   (LOG(CubeWorld::Logger::LogLevel::kError, fmt, ## __VA_ARGS__))
#define LOG_ALWAYS(fmt, ...)  (LOG(CubeWorld::Logger::LogLevel::kAlways, fmt, ## __VA_ARGS__))

}; // namespace Logger

}; // namespace CubeWorld
