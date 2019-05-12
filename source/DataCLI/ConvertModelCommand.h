// By Thomas Steinke

#pragma once

#include <string>
#include <vector>

#include <RGBFileSystem/Paths.h>

namespace CubeWorld
{

class ConvertModelCommand {
public:
   struct Options {

   };

   ConvertModelCommand() {};

   Maybe<std::string> Run(int argc, char** argv);

private:
   Options mOptions;

   std::string mSource;
   std::string mDestination;
};

}; // namespace CubeWorld
