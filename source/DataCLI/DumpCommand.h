// By Thomas Steinke

#pragma once

#include <string>
#include <vector>

#include <Engine/Core/Maybe.h>

namespace CubeWorld
{

class DumpCommand {
public:
   struct Options {

   };

   DumpCommand() {};

   Maybe<std::string> Run(int argc, char** argv);

private:
   Options mOptions;

   std::string mFilename;
   std::string mDestination;
};

}; // namespace CubeWorld
