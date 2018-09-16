// By Thomas Steinke

#pragma once

#include <string>

#include "Format.h"

#if defined(_WIN32)
#define NOMSG
#endif

namespace CubeWorld
{

class Failure {
public:
   Failure() {};
   Failure(const Failure& other) : message(other.message) {};
   Failure(const std::string& message) : message(message) {};

   template <typename ...Args>
   Failure(const std::string& fmt, const Args& ... args) : message(Format::FormatString(fmt, args...)) {};

   const std::string GetMessage() const { return message; }

public:
   Failure WithContext(const std::string& context)
   {
      return Failure(context + ": " + message);
   }

   template <typename ...Args>
   Failure WithContext(const std::string& fmt, const Args& ... args)
   {
      return WithContext(Format::FormatString(fmt, args...));
   }

private:
   std::string message;
};

}; // namespace CubeWorld
