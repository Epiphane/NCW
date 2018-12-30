// By Thomas Steinke

#define _CRT_SECURE_NO_DEPRECATE
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

#include <Engine/Logger/Logger.h>
#include <Engine/Core/Paths.h>
#include <Shared/Helpers/CubeFormat.h>
#include <Engine/Helpers/StringHelper.h>
#include <Shared/Helpers/VoxFormat.h>

#include "Console.h"
#include "ConvertCommand.h"

namespace CubeWorld
{

Maybe<std::string> ConvertCommand::Run(int argc, char** argv)
{
   int argi = 0;
   // Parse options...

   if (argc - argi < 2)
   {
      return Failure{"Usage: dump [opts] source destination"};
   }

   mSource = argv[argi++];
   mDestination = argv[argi++];

   Maybe<std::unique_ptr<Voxel::ModelData>> maybeModel;
   if (StringHelper::EndsWith(mSource, ".cub"))
   {
      LOG_INFO("Determined source file %1 to be a CUB file, opening.", mSource);
      maybeModel = Voxel::CubeFormat::Read(mSource, false);
   }
   else if (StringHelper::EndsWith(mSource, ".vox"))
   {
      LOG_INFO("Determined source file %1 to be a VOX file, opening.", mSource);
      maybeModel = Voxel::VoxFormat::Read(mSource, false);
   }

   if (!maybeModel)
   {
      return maybeModel.Failure().WithContext("Failed to load source file %1", mSource);
   }

   Maybe<void> result;
   if (StringHelper::EndsWith(mDestination, ".cub"))
   {
      LOG_INFO("Writing destination file %1 in CUB format.", mDestination);
      result = Voxel::CubeFormat::Write(mDestination, *(maybeModel->get()));
   }
   else if (StringHelper::EndsWith(mDestination, ".vox"))
   {
      LOG_INFO("Writing destination file %1 in VOX format.", mDestination);
      result = Voxel::VoxFormat::Write(mDestination, *(maybeModel->get()));
   }

   if (!result)
   {
      return result.Failure().WithContext("Failed to write destination file %1", mDestination);
   }

   return mDestination;
}

}; // namespace CubeWorld
