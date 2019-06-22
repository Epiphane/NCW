// By Thomas Steinke

#pragma once

#include <string>

#include <RGBText/Format.h>

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
   Failure(const std::string& message, int failureCode = NO_FAILURE_CODE_SPECIFIED) : message(message), failureCode(failureCode) {};

   template <typename ...Args>
   Failure(const std::string& fmt, const Args& ... args) : message(Format::FormatString(fmt, args...)) {};
   
   template <typename ...Args>
   Failure(int failureCode, const std::string& fmt, const Args& ... args) 
      : message(Format::FormatString(fmt, args...))
      , failureCode(failureCode)
   {};

   const std::string GetMessage() const { return message; }
   int GetFailureCode() const { return failureCode; }

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

   static const int NO_FAILURE_CODE_SPECIFIED = -1;
   
private:
   std::string message;
   int failureCode;  //< Lets you branch code based on what kind of error was returned. Error codes should be scoped to the class they are created in.
                     //<   Defaults to -1 (no code specified)
};

}; // namespace CubeWorld
