#define CATCH_CONFIG_RUNNER
#include "catch.h"

#include <Shared/Helpers/Asset.h>

int main(int argc, char* argv[])
{
   CubeWorld::Asset::SetAssetRootDefault();

   for (int i = 0; i < argc; ++i)
   {
      if (strcmp("--ansi", argv[i]) == 0)
      {
         Catch::Colour::ansi(true);

         // Hide this argument from the Catch library
         argv[i][0] = '\0';
      }
   }

   return Catch::Session().run(argc, argv);
}

// Don't put anything else here, it might increase compile times! :O
