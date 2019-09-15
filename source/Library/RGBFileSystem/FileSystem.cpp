// By Thomas Steinke

#include <deque>

#if CUBEWORLD_PLATFORM_WINDOWS
#include <Windows.h>
#else
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#include <RGBDesignPatterns/Scope.h>
#include <RGBText/Encoding.h>
#include "FileSystem.h"
#include "Paths.h"

namespace CubeWorld
{

using namespace Paths;

Failure DiskFileSystem::TransformPlatformError(const std::string& message)
{
#if CUBEWORLD_PLATFORM_WINDOWS
   Failure err{"Error %1", GetLastError()};
#elif CUBEWORLD_PLATFORM_MACOSX || CUBEWORLD_PLATFORM_LINUX
   Failure err{"Error %1", errno};
#else
#error "Unhandled platform"
#endif

   if (!message.empty())
   {
      return err.WithContext(message);
   }
   return err;
}

///
///
///
std::pair<Maybe<void>, bool> DiskFileSystem::Exists(const std::string& path)
{
   if (path.empty())
   {
      return {Success, false};
   }

#if CUBEWORLD_PLATFORM_WINDOWS
   DWORD ret = ::GetFileAttributesW(Utf8ToWide(path).c_str());
   if (ret != INVALID_FILE_ATTRIBUTES)
   {
      return {Success, true};
   }
   else
   {
      DWORD err = GetLastError();
      if (err == ERROR_PATH_NOT_FOUND || err == ERROR_FILE_NOT_FOUND)
      {
         return {Success, false};
      }
      else
      {
         return {TransformPlatformError("Failed to get attributes"), false};
      }
   }
#elif CUBEWORLD_PLATFORM_MACOSX || CUBEWORLD_PLATFORM_LINUX
   if (access(path.c_str(), 0) == 0)
   {
      struct stat status;
      if (stat(path.c_str(), &status) == 0)
      {
         return {Success, true};
      }
   }
   if (errno == ENOENT)
   {
      return {Success, false};
   }
   return {TransformPlatformError("Failed to get attributes"), false};
#else
#error "Unhandled platform"
#endif
}

///
///
///
std::pair<Maybe<void>, bool> DiskFileSystem::IsDirectory(const std::string& path)
{
#if CUBEWORLD_PLATFORM_WINDOWS
   DWORD ret = ::GetFileAttributesW(Utf8ToWide(path).c_str());
   if (ret == INVALID_FILE_ATTRIBUTES)
   {
      return {TransformPlatformError("Failed to get attributes"), false};
   }
   return {Success, (ret & FILE_ATTRIBUTE_DIRECTORY) != 0};
#elif CUBEWORLD_PLATFORM_MACOSX || CUBEWORLD_PLATFORM_LINUX
   if (access(path.c_str(), 0) == 0)
   {
      struct stat status;
      if (0 == stat(path.c_str(), &status))
      {
         return {Success, (status.st_mode & S_IFDIR) != 0};
      }
   }
   return {Failure{"Failed to access path: Error %1", errno}, false};
#else
#error "Unhandled platform"
#endif
}

///
///
///
Maybe<void> DiskFileSystem::MakeDirectory(const std::string& path)
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
      std::wstring normalizedW = Utf8ToWide(normalized);
      DWORD dwAttrib = GetFileAttributesW(normalizedW.c_str());

      if (dwAttrib == INVALID_FILE_ATTRIBUTES)
      {
         DWORD err = GetLastError();
         if (err == ERROR_PATH_NOT_FOUND || err == ERROR_FILE_NOT_FOUND)
         {
            if (CreateDirectoryW(normalizedW.c_str(), nullptr) == 0 && GetLastError() != ERROR_ALREADY_EXISTS)
            {
               return TransformPlatformError(FormatString("Failed to create directory %1", normalized));
            }
         }
         else
         {
            return TransformPlatformError(FormatString("Failed to get attributes on %1", normalized));
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

///
///
///
Maybe<std::vector<DiskFileSystem::FileEntry>> DiskFileSystem::ListDirectory(
   const std::string& base,
   bool includeDirectories,
   bool recursive
)
{
   std::vector<DiskFileSystem::FileEntry> result;

   std::deque<std::string> paths{""};
   while (!paths.empty())
   {
      std::string path = paths.front();
      paths.pop_front();

#if defined CUBEWORLD_PLATFORM_WINDOWS
      std::wstring pathW = Utf8ToWide(Paths::Join(base, path) + "/*");

      WIN32_FIND_DATAW info;
      HANDLE handle = FindFirstFileW(pathW.c_str(), &info);
      if (handle == INVALID_HANDLE_VALUE)
      {
         return result;
      }

      CUBEWORLD_SCOPE_EXIT([&] {FindClose(handle); });
      do
      {
         if (wcscmp(info.cFileName, L".") == 0 || wcscmp(info.cFileName, L"..") == 0)
         {
            continue;
         }

         DiskFileSystem::FileEntry entry;
         entry.name = path + "/" + WideToUtf8(info.cFileName);
         entry.isDirectory = (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
         entry.size = 0;
         if (entry.isDirectory)
         {
            if (recursive)
            {
               paths.push_back(entry.name);
            }
            if (includeDirectories)
            {
               result.push_back(std::move(entry));
            }
         }
         else
         {
            entry.size = (uint64_t(info.nFileSizeHigh) << 32) + info.nFileSizeLow;
            result.push_back(std::move(entry));
         }
      } while (FindNextFileW(handle, &info) != 0);

      DWORD error = GetLastError();
      if (error != ERROR_NO_MORE_FILES)
      {
         return TransformPlatformError("Failed enumerating files");
      }
#elif CUBEWORLD_PLATFORM_MACOSX || CUBEWORLD_PLATFORM_LINUX
      DIR *dir;
      dir = opendir(Paths::Join(base, path).c_str());
      if (dir == nullptr)
      {
         return TransformPlatformError(FormatString("Failed opening %1", Paths::Join(base, path)));
      }


      for (struct dirent *info = readdir(dir); info != nullptr; info = readdir(dir))
      {
         // Ignore . and ..
         if (strcmp(info->d_name, ".") == 0 || strcmp(info->d_name, "..") == 0)
         {
            continue;
         }

         DiskFileSystem::FileEntry entry;
         entry.name = path + "/" + info->d_name;
         entry.size = 0;

         struct stat status;
         if (0 == stat(Paths::Join(base, entry.name).c_str(), &status))
         {
            entry.isDirectory = (status.st_mode & S_IFDIR) != 0;
            entry.size = status.st_size;
         }

         if (entry.isDirectory)
         {
            if (recursive)
            {
               paths.push_back(entry.name);
            }
            if (includeDirectories)
            {
               result.push_back(std::move(entry));
            }
         }
         else
         {
            result.push_back(std::move(entry));
         }
      }
      closedir(dir);
#else
#error "Unhandled platform"
#endif
   }

   return result;
}

///
///
///
Maybe<FileSystem::FileHandle> DiskFileSystem::OpenFileRead(const std::string& path)
{
#if defined CUBEWORLD_PLATFORM_WINDOWS
   HANDLE result = CreateFileW(Utf8ToWide(path).c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

   if (result == INVALID_HANDLE_VALUE)
   {
      return TransformPlatformError("Failed opening file for read");
   }

   return result;
#elif CUBEWORLD_PLATFORM_MACOSX || CUBEWORLD_PLATFORM_LINUX
   int file = open(path.c_str(), O_RDONLY);
   if (file == -1)
   {
      return TransformPlatformError("Failed opening file for read");
   }
   return file;
#else
#error "Unhandled platform"
#endif
}

///
///
///
Maybe<void> DiskFileSystem::ReadFile(FileHandle handle, void* data, size_t size)
{
#if defined CUBEWORLD_PLATFORM_WINDOWS
   DWORD numRead;
   if (::ReadFile(handle, data, (DWORD)size, &numRead, nullptr) == 0)
   {
      return TransformPlatformError("Failed reading file");
   }

   if (numRead != size)
   {
      return Failure{"Tried to read %1 bytes, but only got %2", size, numRead};
   }

   return Success;
#elif CUBEWORLD_PLATFORM_MACOSX || CUBEWORLD_PLATFORM_LINUX
   ssize_t numRead = read(handle, data, size);
   if ((size_t)numRead != size)
   {
      return Failure{"Tried to read %1 bytes, but only got %2", size, numRead};
   }
   return Success;
#else
#error "Unhandled platform"
#endif
}

///
///
///
Maybe<std::string> DiskFileSystem::ReadEntireFile(const std::string& path)
{
   std::string result;
   Maybe<FileHandle> maybeHandle = OpenFileRead(path);
   if (!maybeHandle)
   {
      return maybeHandle.Failure();
   }

   CUBEWORLD_SCOPE_EXIT([&] { CloseFile(*maybeHandle); });

   int64_t fSize = 0;
#if defined CUBEWORLD_PLATFORM_WINDOWS
   LARGE_INTEGER fileSize;
   if (GetFileSizeEx(*maybeHandle, &fileSize) == 0)
   {
      return TransformPlatformError("Failed getting file size");
   }

   fSize = int64_t(fileSize.QuadPart);
#elif CUBEWORLD_PLATFORM_MACOSX || CUBEWORLD_PLATFORM_LINUX
   fSize = lseek(*maybeHandle, 0, SEEK_END);
   if (fSize < 0)
   {
      return TransformPlatformError("Failed getting file size");
   }

   if (lseek(*maybeHandle, 0, SEEK_SET) < 0)
   {
      return TransformPlatformError("Failed resetting to start of file");
   }
#else
#error "Unhandled platform"
#endif
   result.resize(static_cast<uint32_t>(fSize));
   Maybe<void> read = ReadFile(*maybeHandle, result.data(), (size_t)fSize);
   if (!read)
   {
      return read.Failure();
   }

   return result;
}

///
///
///
Maybe<DiskFileSystem::FileHandle> DiskFileSystem::OpenFileWrite(const std::string& path)
{
#if defined CUBEWORLD_PLATFORM_WINDOWS
   HANDLE result = CreateFileW(Utf8ToWide(path).c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

   if (result == INVALID_HANDLE_VALUE)
   {
      return TransformPlatformError("Failed opening file for write");
   }

   return result;
#elif CUBEWORLD_PLATFORM_MACOSX || CUBEWORLD_PLATFORM_LINUX
   int file = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC);
   if (file == -1)
   {
      return TransformPlatformError("Failed opening file for read");
}
   return file;
#else
#error "Unhandled platform"
#endif
}

///
///
///
Maybe<void> DiskFileSystem::WriteFile(FileHandle handle, void* data, size_t size)
{
#if defined CUBEWORLD_PLATFORM_WINDOWS
   DWORD numWritten;
   if (::WriteFile(handle, data, (DWORD)size, &numWritten, nullptr) == 0)
   {
      return TransformPlatformError("Failed writing to file");
   }

   if (numWritten != size)
   {
      return Failure{"Tried to write %1 bytes, but only wrote %2", size, numWritten};
   }

   return Success;
#elif CUBEWORLD_PLATFORM_MACOSX || CUBEWORLD_PLATFORM_LINUX
   ssize_t numWritten = write(handle, data, size);
   if ((size_t)numWritten != size)
   {
      return Failure{"Tried to write %1 bytes, but only wrote %2", size, numWritten};
   }
   return Success;
#else
#error "Unhandled platform"
#endif
}

///
///
///
Maybe<void> DiskFileSystem::WriteFile(const std::string& path, const std::string& data)
{
   Maybe<FileHandle> maybeHandle = OpenFileWrite(path);
   if (!maybeHandle)
   {
      return maybeHandle.Failure().WithContext("Failed opening file for write");
   }

   CUBEWORLD_SCOPE_EXIT([&] { CloseFile(*maybeHandle); });

   return WriteFile(*maybeHandle, (uint8_t*)data.c_str(), data.size());
}

///
///
///
Maybe<void> DiskFileSystem::SeekFile(FileHandle handle, Seek method, int64_t dist)
{
#if defined CUBEWORLD_PLATFORM_WINDOWS
   LARGE_INTEGER distance;
   distance.QuadPart = dist;
   DWORD ptr = ::SetFilePointer(handle, (LONG)distance.LowPart, distance.HighPart != 0 ? &distance.HighPart : nullptr, method);
   if (ptr == INVALID_SET_FILE_POINTER)
   {
      return TransformPlatformError("Failed setting pointer in file");
   }
#elif CUBEWORLD_PLATFORM_MACOSX || CUBEWORLD_PLATFORM_LINUX
   if (lseek(handle, dist, method) != 0)
   {
      return TransformPlatformError("Failed setting pointer in file");
   }
#else
#error "Unhandled platform"
#endif
   return Success;
}

///
///
///
Maybe<void> DiskFileSystem::CloseFile(FileHandle handle)
{
#if defined CUBEWORLD_PLATFORM_WINDOWS
   if (CloseHandle(handle) == 0)
   {
      return TransformPlatformError();
   }
#elif CUBEWORLD_PLATFORM_MACOSX || CUBEWORLD_PLATFORM_LINUX
   if (close(handle) != 0)
   {
      return TransformPlatformError();
   }
#else
#error "Unhandled platform"
#endif
   return Success;
}

}; // namespace CubeWorld
