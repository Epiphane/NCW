// By Thomas Steinke

#include <sqlite3.h>

#include <RGBDesignPatterns/Scope.h>
#include <RGBFileSystem/FileSystem.h>
#include <RGBFileSystem/Paths.h>

#include "Console.h"
#include "Database.h"
#include "DumpCommand.h"
#include "Scrambler.h"

namespace CubeWorld
{

Maybe<std::string> DumpCommand::Run(int argc, char** argv)
{
   int argi = 0;
   // Parse options...

   if (argc - argi < 2)
   {
      return Failure{"Usage: dump [opts] filename destination"};
   }

   mFilename = argv[argi++];
   mDestination = argv[argi++];

   if (Maybe<void> result = DiskFileSystem{}.MakeDirectory(mDestination); !result)
   {
      return result.Failure().WithContext("Failed to make directory {path}", mDestination);
   }

   std::string databasePath = Paths::Canonicalize(mFilename);
   Maybe<std::unique_ptr<Database>> maybeDb = Database::OpenRead(databasePath);
   if (!maybeDb)
   {
      return maybeDb.Failure().WithContext("Failed to open database at {path}", databasePath);
   }

   std::unique_ptr<Database> database = std::move(*maybeDb);

   DiskFileSystem fs{};
   database->EnumerateBlobs([&](const Database::Blob& blob) -> Maybe<void> {
      std::string path = Paths::Join(mDestination, blob.key);
      Maybe<FileSystem::FileHandle> maybeHandle = fs.OpenFileWrite(path);
      if (!maybeHandle)
      {
         return maybeHandle.Failure().WithContext("Failed opening {path} for writing", path);
      }

      FileSystem::FileHandle handle = *maybeHandle;
      CUBEWORLD_SCOPE_EXIT([&]() { fs.CloseFile(handle); })

      Scrambler scrambler{};

      scrambler.Unscramble((char*)&blob.value[0], sizeof(uint8_t) * blob.value.size());

      if (Maybe<void> write = fs.WriteFile(handle, (void*)&blob.value[0], sizeof(uint8_t) * blob.value.size()); !write)
      {
         return write.Failure();
      }

      LOG_ALWAYS("Exported {key}", blob.key);

      return Success;
   });

   return std::string("Done.");
}

}; // namespace CubeWorld
