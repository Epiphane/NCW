// By Thomas Steinke

#define _CRT_SECURE_NO_DEPRECATE
#include <stdlib.h>
#include <sqlite3.h>

#include <Engine/Core/Paths.h>

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

   if (Maybe<void> result = Paths::MakeDirectory(mDestination); !result)
   {
      return result.Failure().WithContext("Failed to make directory %1", mDestination);
   }

   std::string databasePath = Paths::Canonicalize(mFilename);
   Maybe<std::unique_ptr<Database>> maybeDb = Database::OpenRead(databasePath);
   if (!maybeDb)
   {
      return maybeDb.Failure().WithContext("Failed to open database at %1", databasePath);
   }

   std::unique_ptr<Database> database = std::move(*maybeDb);

   database->EnumerateBlobs([&](const Database::Blob& blob) -> Maybe<void> {
      std::string path = Paths::Join(mDestination, blob.key);
      FILE* file = fopen(path.c_str(), "wb");

      if (file == nullptr)
      {
         return Failure{"Failed to open %1 for writing", path};
      }

      Scrambler scrambler{};

      scrambler.Unscramble((char*)&blob.value[0], sizeof(uint8_t) * blob.value.size());

      size_t written = fwrite(&blob.value[0], sizeof(uint8_t), blob.value.size(), file);
      if (written != blob.value.size())
      {
         return Failure{"Only wrote %1 out of %2 bytes", written, blob.value.size()};
      }

      fclose(file);

      LOG_ALWAYS("Exported %1", blob.key);

      return Success;
   });

   return std::string("Done.");
}

}; // namespace CubeWorld
