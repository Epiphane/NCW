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

#include "Paths.h"

namespace CubeWorld
{

namespace Paths
{

// TODO StringHelper
namespace
{

bool DirectoryExists(const char* path)
{
#if defined(CUBEWORLD_PLATFORM_WINDOWS)
   DWORD ret = ::GetFileAttributes(path);
   return ret != INVALID_FILE_ATTRIBUTES && (ret & FILE_ATTRIBUTE_DIRECTORY) != 0;
#elif (defined(CUBEWORLD_PLATFORM_MACOSX) || defined(CUBEWORLD_PLATFORM_LINUX))
   if (access(path, 0) == 0)
   {
      struct stat status;
      if (0 == stat(path, &status)
         && (status.st_mode & S_IFDIR))
      {
         return true;
      }
   }
   return false;
#else
#error "Unhandled platform"
#endif
}

std::vector<std::string> Split(const std::string& path)
{
   std::vector<std::string> result;
   if (path.empty())
   {
      return result;
   }

   size_t begin = 0, end = 0;
   while (end != std::string::npos)
   {
      end = path.find('/', begin + 1);
      result.push_back(path.substr(begin, end - begin));
      begin = end + 1;
   }

   return result;
}

std::string Join(const std::vector<std::string>& parts)
{
   std::string result;
   if (parts.empty())
   {
      return result;
   }

   for (int i = 0; i < parts.size(); ++i)
   {
      if (i > 0)
      {
         result += "/";
      }
      result += parts[i];
   }

   return result;
}

}; // private namespace

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

   std::vector<std::string> parts = Split(absolute);
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

   std::string result = Join(resolved);
   if (resolved.size() == 1)
   {
      return result + "/";
   }
   else
   {
      return result;
   }
}

Maybe<void> MakeDirectory(const std::string& path)
{
   std::string normalized = Normalize(path);

   if (!normalized.empty() && normalized.back() != '/')
   {
      // Add a final slash so the loop below creates the leaf directory.
      normalized.push_back('/');
   }

   std::string::size_type slash = 0;

   if (IsAbsolute(normalized))
   {
      // Skip the first slash so we don't try to create the root directory.
      slash = normalized.find('/');
   }

   while ((slash = normalized.find('/', slash + 1)) != std::string::npos)
   {
      normalized[slash] = '\0';

#if CUBEWORLD_PLATFORM_WINDOWS
      DWORD dwAttrib = GetFileAttributes(normalized.c_str());

      if (dwAttrib == INVALID_FILE_ATTRIBUTES)
      {
         if (GetLastError() == ERROR_PATH_NOT_FOUND || GetLastError() == ERROR_FILE_NOT_FOUND)
         {
            if (!CreateDirectory(normalized.c_str(), nullptr) && GetLastError() != ERROR_ALREADY_EXISTS)
            {
               return Failure{"Failed to create directory %1: Error %2", normalized.c_str(), GetLastError()};
            }
         }
         else
         {
            return Failure{"Failed to get attributes on %1: Error %2", normalized.c_str(), GetLastError()};
         }
      }
      else if ((dwAttrib & FILE_ATTRIBUTE_DIRECTORY) == 0)
      {
         return Failure{"%1 is not a directory", normalized};
      }
#else
      if (mkdir(normalized.c_str(), 0755) != 0 && errno != EEXIST)
      {
         return Failure{"Failed to create directory: Error %1", errno};
      }
#endif

      normalized[slash] = '/';
   }

   return Success;
}

std::string GetWorkingDirectory()
{
#if CUBEWORLD_PLATFORM_WINDOWS
   TCHAR buf[256];
   GetCurrentDirectory(256, buf);
   return Normalize(std::string(buf));
#else
   return Normalize(std::string(getenv("PWD")));
#endif
}

}; // namespace Paths

}; // namespace CubeWorld
