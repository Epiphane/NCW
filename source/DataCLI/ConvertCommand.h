// By Thomas Steinke

#pragma once

#include <string>
#include <vector>

#include <RGBFileSystem/Paths.h>

namespace CubeWorld
{

class ConvertCommand {
public:
   struct Options {

   };

   ConvertCommand() {};

   Maybe<std::string> Run(int argc, char** argv);

private:
   Options mOptions;

   std::string mSource;
   std::string mDestination;
};

}; // namespace CubeWorld
