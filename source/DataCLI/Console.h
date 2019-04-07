// By Thomas Steinke

#pragma once

#include <RGBLogger/Logger.h>

namespace CubeWorld
{

class Console {
public:
   template <typename... Args>
   const static inline void Log(std::string_view fmt, const Args& ... args) { CubeWorld::Logger::LogManager::Instance()->Log(Format::FormatString(fmt, args...).c_str()); }
};

}; // namespace CubeWorld
