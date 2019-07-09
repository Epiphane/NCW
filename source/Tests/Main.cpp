#define CATCH_CONFIG_RUNNER
#include "catch.h"

#include <Shared/Helpers/Asset.h>

int main(int argc, char* argv[])
{
   CubeWorld::Asset::SetAssetRootDefault();

   return Catch::Session().run(argc, argv);
}

// Don't put anything else here, it might increase compile times! :O
