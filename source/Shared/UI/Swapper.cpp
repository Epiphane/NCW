// By Thomas Steinke

#include <cassert>

#include "Swapper.h"

namespace CubeWorld
{

namespace UI
{

void Swapper::Swap(Engine::UIRoot* window)
{
   mCurrent = window;
}

}; // namespace UI

}; // namespace CubeWorld
