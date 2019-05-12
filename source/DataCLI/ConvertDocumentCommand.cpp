// By Thomas Steinke

#include <RGBBinding/BindingProperty.h>
#include <RGBLogger/Logger.h>
#include <RGBNetworking/JSONSerializer.h>
#include <RGBNetworking/YAMLSerializer.h>
#include <RGBText/StringHelper.h>

#include "ConvertDocumentCommand.h"

namespace CubeWorld
{

Maybe<std::string> ConvertDocumentCommand::Run(int argc, char** argv)
{
   int argi = 0;
   // Parse options...

   if (argc - argi < 2)
   {
      return Failure{"Usage: convert-document [opts] source destination"};
   }

   mSource = argv[argi++];
   mDestination = argv[argi++];

   Maybe<BindingProperty> data;
   if (StringHelper::EndsWith(mSource, ".json"))
   {
      data = JSONSerializer::DeserializeFile(mSource);
   }
   else if (StringHelper::EndsWith(mSource, ".yaml"))
   {
      data = YAMLSerializer::DeserializeFile(mSource);
   }

   if (!data)
   {
      return data.Failure().WithContext("Failed to load source file %1", mSource);
   }

   Maybe<void> result;
   if (StringHelper::EndsWith(mDestination, ".json"))
   {
      result = JSONSerializer::SerializeFile(mDestination, *data);
   }
   else if (StringHelper::EndsWith(mDestination, ".yaml"))
   {
      result = YAMLSerializer::SerializeFile(mDestination, *data);
   }

   if (!result)
   {
      return result.Failure().WithContext("Failed to write destination file %1", mDestination);
   }

   return mDestination;
}

}; // namespace CubeWorld
