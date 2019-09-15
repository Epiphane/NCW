// By Thomas Steinke

#pragma once

#include <string>

#include <RGBText/Format.h>
#include <RGBLogger/Logger.h>

#if defined(_WIN32)
#define NOMSG
#undef GetMessage
#endif

namespace CubeWorld
{

class Failure {
public:
   Failure() : failureCode(0) {};
   Failure(const Failure& other) : message(other.message), failureCode(other.failureCode) {};

   template <typename ...Args>
   Failure(const std::string& fmt, const Args& ... args)
      : message(FormatString(fmt, args...))
      , failureCode(NO_FAILURE_CODE_SPECIFIED)
   {};

   template <typename ...Args>
   Failure(int failureCode, const std::string& fmt, const Args& ... args)
      : message(FormatString(fmt, args...))
      , failureCode(failureCode)
   {};

   const std::string GetMessage() const { return message; }
   void Log() const { LOG_ERROR("%1", GetMessage()); }
   int GetFailureCode() const { return failureCode; }

public:
   Failure WithContext(const std::string& context)
   {
      return Failure(context + ": " + message);
   }

   template <typename ...Args>
   Failure WithContext(const std::string& fmt, const Args& ... args)
   {
      return WithContext(FormatString(fmt, args...));
   }

   static const int NO_FAILURE_CODE_SPECIFIED = -1;

private:
   std::string message;
   int failureCode;  //< Lets you branch code based on what kind of error was returned. Error codes should be scoped to the class they are created in.
                     //<   Defaults to -1 (no code specified)
};

}; // namespace CubeWorld
