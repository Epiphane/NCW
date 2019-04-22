// By Thomas Steinke

#include <algorithm>
#include <vector>

#if CUBEWORLD_PLATFORM_WINDOWS
#include <Windows.h>
#else
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include <RGBText/StringHelper.h>
#include <RGBText/Encoding.h>
#include "Paths.h"

namespace CubeWorld
{

namespace Paths
{

bool IsAbsolute(const std::string& path)
{
   return (path.size() >= 1 && path[0] == '/') // POSIX
       || (path.size() >= 2 && path[1] == ':'); // Windows
}

bool IsRoot(const std::string& path)
{
   return (path == "/") // POSIX
      || (path.size() == 2 && path[1] == ':') // Windows
      || (path.size() == 3 && path[1] == ':' && (path[2] == '/' || path[2] == '\\')); // Windows
}

// NOTE: This is intentionally std::string, instead of const std::string&, so that we can modify it for the return value.
std::string Normalize(std::string path)
{
   std::replace(path.begin(), path.end(), '\\', '/');

   return path;
}

std::string Join(const std::string& part1, const std::string& part2)
{
   std::string normalized1 = Normalize(part1);
   std::string normalized2 = Normalize(part2);

   if (normalized1.empty())
   {
      return normalized2;
   }
   else if (normalized2.empty())
   {
      return normalized1;
   }
   else
   {
      return normalized1 + '/' + normalized2;
   }
}

std::string Canonicalize(const std::string& path)
{
   std::string absolute = IsAbsolute(path)
      ? Normalize(path)
      : Join(GetWorkingDirectory(), path);

   std::vector<std::string> parts = StringHelper::Split(absolute);
   std::vector<std::string> resolved;
   for (const std::string& part : parts)
   {
      if (part == "..")
      {
         if (resolved.size() > 1)
         {
            resolved.pop_back();
         }
      }
      else if (part != ".")
      {
         resolved.push_back(part);
      }
   }

   std::string result = StringHelper::Join(resolved);
   if (resolved.size() == 1)
   {
      return result + "/";
   }
   else
   {
      return result;
   }
}

std::string GetRelativePath(const std::string& path_, const std::string& base_)
{
   const std::string path = Canonicalize(path_);
   const std::string base = Canonicalize(base_);

   std::vector<std::string> baseParts = StringHelper::Split(base);
   std::vector<std::string> pathParts = StringHelper::Split(path);

   size_t common = 0;
   for (;common < baseParts.size() && common < pathParts.size(); ++common)
   {
      if (baseParts[common] != pathParts[common])
      {
         break;
      }
   }

   // Example state:
   // path: /path/to/Assets/Skeletons/character
   // base: /path/to/Assets/Models
   // common: 3
   // want: ../Skeletons/Character
   std::string result = "";
   for (size_t i = 0; i < baseParts.size() - common; ++i)
   {
      result += "../";
   }

   for (size_t i = common; i < pathParts.size(); ++i)
   {
      result += pathParts[i] + "/";
   }

   // Remove trailing slash
   if (result.size() > 0)
   {
      result.erase(result.size() - 1);
   }
   return result;
}

std::string GetBasename(const std::string& path)
{
   std::string normalized = Normalize(path);
   if (auto dot = normalized.rfind('.'); dot != std::string::npos)
   {
      normalized.erase(dot);
   }
   
   if (auto last = normalized.rfind('/'); last != std::string::npos)
   {
      return normalized.substr(last + 1);
   }
   else
   {
      return normalized;
   }
}

std::string GetFilename(const std::string& path)
{
   std::string normalized = Normalize(path);
   if (auto last = normalized.rfind('/'); last != std::string::npos)
   {
      return path.substr(last + 1);
   }
   else
   {
      return path;
   }
}

std::string GetDirectory(const std::string& path)
{
   std::string normalized = Normalize(path);
   if (auto last = normalized.rfind('/'); last != std::string::npos)
   {
      return normalized.substr(0, last);
   }
   else
   {
      return normalized;
   }
}

std::string GetWorkingDirectory()
{
#if CUBEWORLD_PLATFORM_WINDOWS
   wchar_t buf[256];
   GetCurrentDirectoryW(256, buf);
   return Normalize(WideToUtf8(buf));
#else
   return Normalize(std::string(getenv("PWD")));
#endif
}

}; // namespace Paths

}; // namespace CubeWorld
