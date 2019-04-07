// By Thomas Steinke

#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include <RGBFileSystem/Paths.h>

namespace CubeWorld
{

class Scrambler {
public:
   Scrambler() {};
   ~Scrambler() {};
   
   void Scramble(char* data, size_t size);
   void Unscramble(char* data, size_t size);

private:
   const static uint32_t offsets[];
};

}; // namespace CubeWorld
