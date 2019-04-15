// By Thomas Steinke

#include <sstream>

#include "StringHelper.h"

namespace CubeWorld
{

namespace StringHelper
{

bool EndsWith(const std::string& source, const std::string pattern)
{
   if (source.size() < pattern.size())
   {
      return false;
   }
   return source.compare(source.size() - pattern.size(), pattern.size(), pattern) == 0;
}

bool StartsWith(const std::string& source, const std::string pattern)
{
   if (source.size() < pattern.size())
   {
      return false;
   }
   return source.compare(0, pattern.size(), pattern) == 0;
}
   
std::vector<std::string> Split(const std::string& path, char delim)
{
   std::vector<std::string> result;
   if (path.empty())
   {
      return result;
   }

   size_t begin = 0, end = 0;
   while (end != std::string::npos)
   {
      end = path.find(delim, begin + 1);
      result.push_back(path.substr(begin, end - begin));
      begin = end + 1;
   }

   return result;
}

std::string Join(const std::vector<std::string>& parts, std::string delim)
{
   if (parts.empty())
   {
      return "";
   }
   std::ostringstream result;

   for (int i = 0; i < parts.size(); ++i)
   {
      if (i > 0)
      {
         result << delim;
      }
      result << parts[i];
   }

   return result.str();
}

}; // namespace StringHelper

}; // namespace CubeWorld