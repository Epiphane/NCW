// By Thomas Steinke

#define _CRT_SECURE_NO_DEPRECATE
#include <stdlib.h>
#include <sqlite3.h>

#include <RGBLogger/Logger.h>
#include <RGBFileSystem/Paths.h>
#include <RGBText/StringHelper.h>
#include <Shared/Helpers/CubeFormat.h>
#include <Shared/Helpers/VoxFormat.h>

#include "Console.h"
#include "ConvertModelCommand.h"

namespace CubeWorld
{

Maybe<std::string> ConvertModelCommand::Run(int argc, char** argv)
{
   int argi = 0;
   // Parse options...

   if (argc - argi < 2)
   {
      return Failure{"Usage: convert-model [opts] source destination"};
   }

   mSource = argv[argi++];
   mDestination = argv[argi++];

   Maybe<std::unique_ptr<Voxel::ModelData>> maybeModel;
   if (StringHelper::EndsWith(mSource, ".cub"))
   {
      maybeModel = Voxel::CubeFormat::Read(mSource, false);
   }
   else if (StringHelper::EndsWith(mSource, ".vox"))
   {
      maybeModel = Voxel::VoxFormat::Read(mSource, false);
   }

   if (!maybeModel)
   {
      return maybeModel.Failure().WithContext("Failed to load source file %1", mSource);
   }

   Maybe<void> result;
   if (StringHelper::EndsWith(mDestination, ".cub"))
   {
      result = Voxel::CubeFormat::Write(mDestination, *(maybeModel->get()));
   }
   else if (StringHelper::EndsWith(mDestination, ".vox"))
   {
      result = Voxel::VoxFormat::Write(mDestination, *(maybeModel->get()));
   }

   if (!result)
   {
      return result.Failure().WithContext("Failed to write destination file %1", mDestination);
   }

   return mDestination;
}

}; // namespace CubeWorld
