// By Thomas Steinke

#pragma once

#include <memory>

#include <RGBDesignPatterns/Singleton.h>
#include <RGBFileSystem/FileSystem.h>

namespace CubeWorld
{

namespace Engine
{

class FileSystemProvider : public Singleton<FileSystemProvider>, public FileSystem
{
public:
   // Lifecycle
   void Use(std::unique_ptr<FileSystem>&& fs);

   FileSystem* get();
   FileSystem& operator->();

private:
   void EnsureFileSystem();

public:
   // Convenience
   std::pair<Maybe<void>, bool> Exists(const std::string& path) override
   {
      return get()->Exists(path);
   }

   std::pair<Maybe<void>, bool> IsDirectory(const std::string& path) override
   {
      return get()->IsDirectory(path);
   }

   Maybe<void> MakeDirectory(const std::string& path) override
   {
      return get()->MakeDirectory(path);
   }

   Maybe<std::vector<FileEntry>> ListDirectory(
      const std::string& path,
      bool includeDirectories,
      bool recursive
   ) override
   {
      return get()->ListDirectory(path, includeDirectories, recursive);
   }

   Maybe<FileHandle> OpenFileRead(const std::string& path) override
   {
      return get()->OpenFileRead(path);
   }

   Maybe<void> ReadFile(FileHandle handle, void* data, size_t size) override
   {
      return get()->ReadFile(handle, data, size);
   }

   Maybe<std::string> ReadEntireFile(const std::string& path) override
   {
      return get()->ReadEntireFile(path);
   }

   Maybe<FileHandle> OpenFileWrite(const std::string& path) override
   {
      return get()->OpenFileWrite(path);
   }

   Maybe<void> WriteFile(FileHandle handle, void* data, size_t size) override
   {
      return get()->WriteFile(handle, data, size);
   }

   Maybe<void> WriteFile(const std::string& path, const std::string& data) override
   {
      return get()->WriteFile(path, data);
   }

   Maybe<void> SeekFile(FileHandle handle, Seek method, int64_t dist) override
   {
      return get()->SeekFile(handle, method, dist);
   }

   Maybe<void> CloseFile(FileHandle handle) override
   {
      return get()->CloseFile(handle);
   }

private:
   std::unique_ptr<FileSystem> fs;
};

}; // namespace Engine

}; // namespace CubeWorld
