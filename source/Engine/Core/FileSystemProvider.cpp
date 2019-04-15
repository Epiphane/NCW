// By Thomas Steinke

#include "FileSystemProvider.h"

namespace CubeWorld
{

namespace Engine
{

void FileSystemProvider::Use(std::unique_ptr<FileSystem>&& filesystem)
{
   fs = std::move(filesystem);
}

FileSystem* FileSystemProvider::get()
{
   EnsureFileSystem();
   return fs.get();
}

FileSystem& FileSystemProvider::operator->()
{
   EnsureFileSystem();
   return *fs;
}

void FileSystemProvider::EnsureFileSystem()
{
   if (!fs)
   {
      fs.reset(new DiskFileSystem());
   }
}

}; // namespace Engine

}; // namespace CubeWorld
