// By Thomas Steinke

#include <cassert>

#include "UISwapper.h"

namespace CubeWorld
{

namespace Editor
{

void UISwapper::Swap(Engine::UIRoot* window)
{
   mCurrent = window;
}

}; // namespace Editor

}; // namespace CubeWorld
