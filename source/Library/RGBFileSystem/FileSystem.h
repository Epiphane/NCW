// By Thomas Steinke

#pragma once

#include <string>
#include <vector>

#include <RGBDesignPatterns/Maybe.h>

namespace CubeWorld
{

class FileSystem
{
public:
   struct FileEntry {
      std::string name;
      bool isDirectory = false;
      uint64_t size = 0;
   };

#if defined CUBEWORLD_PLATFORM_WINDOWS
   using FileHandle = void*;
#else
   using FileHandle = int;
#endif

public:
   //
   // Returns whether a file or directory exists at the specified path.
   //
   virtual std::pair<Maybe<void>, bool> Exists(const std::string& path) = 0;

   //
   // Returns whether the specified path refers to a directory.
   //
   virtual std::pair<Maybe<void>, bool> IsDirectory(const std::string& path) = 0;

   //
   // Make a directory, and all intermediate paths as necessary.
   //
   virtual Maybe<void> MakeDirectory(const std::string& path) = 0;

   //
   // List all the files in a directory
   //
   virtual Maybe<std::vector<FileEntry>> ListDirectory(
      const std::string& path,
      bool includeDirectories,
      bool recursive
   ) = 0;

   //
   // Open a file for reading.
   //
   virtual Maybe<FileHandle> OpenFileRead(const std::string& path) = 0;

   //
   // Write data into a file
   //
   virtual Maybe<void> ReadFile(FileHandle handle, void* data, size_t size) = 0;

   //
   // Open a file and read its contents completely.
   //
   virtual Maybe<std::string> ReadEntireFile(const std::string& path) = 0;

   //
   // Open a file for reading.
   //
   virtual Maybe<FileHandle> OpenFileWrite(const std::string& path) = 0;

   //
   // Write data into a file
   //
   virtual Maybe<void> WriteFile(FileHandle handle, void* data, size_t size) = 0;

   //
   // Open a file and read its contents completely.
   //
   virtual Maybe<void> WriteFile(const std::string& path, const std::string& data) = 0;

   //
   // Seek to a new position within a file
   //
   // Win:     FILE_BEGIN = 0, FILE_CURRENT = 1, FILE_END = 2
   // OSX:     SEEK_SET = 0,   SEEK_CUR = 1,     SEEK_END = 2
   enum Seek { BEGIN = 0,      CURRENT = 1,      END = 2 };

   virtual Maybe<void> SeekFile(FileHandle handle, Seek method, int64_t dist) = 0;

   //
   // Close a file handle.
   //
   virtual Maybe<void> CloseFile(FileHandle handle) = 0;
};

//
// Specific disk implementation of FileSystem. This allows for accessing the disk in the way you'd expect to.
//
class DiskFileSystem : public FileSystem
{
public:
   //
   //
   //
   std::pair<Maybe<void>, bool> Exists(const std::string& path) override;
   
   //
   //
   //
   std::pair<Maybe<void>, bool> IsDirectory(const std::string& path) override;
   
   //
   //
   //
   Maybe<void> MakeDirectory(const std::string& path) override;
   
   //
   //
   //
   Maybe<std::vector<FileEntry>> ListDirectory(
      const std::string& path,
      bool includeDirectories,
      bool recursive
   ) override;

   //
   //
   //
   Maybe<FileHandle> OpenFileRead(const std::string& path) override;

   //
   //
   //
   Maybe<void> ReadFile(FileHandle handle, void* data, size_t size) override;

   //
   //
   //
   Maybe<std::string> ReadEntireFile(const std::string& path) override;

   //
   //
   //
   Maybe<FileHandle> OpenFileWrite(const std::string& path) override;

   //
   //
   //
   Maybe<void> WriteFile(FileHandle handle, void* data, size_t size) override;

   //
   //
   //
   Maybe<void> WriteFile(const std::string& path, const std::string& data) override;

   //
   //
   //
   Maybe<void> SeekFile(FileHandle handle, Seek method, int64_t dist) override;

   //
   // Close a file handle.
   //
   Maybe<void> CloseFile(FileHandle handle) override;

private:
   //
   //
   //
   Failure TransformPlatformError(const std::string& message = "");
};

}; // namespace CubeWorld
